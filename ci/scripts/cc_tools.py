#!/usr/bin/env python3
"""ClaudeClang CI 보조 도구.

하위 명령:
  tests    <junit.xml>                       GoogleTest 결과의 성공률 100% 검사
  coverage <summary.json> [--require m=p ..] llvm-cov export 요약 출력과 임계값 검사
  hitl     <project_dir> <dir> [<dir> ..]    Doxygen 주석의 Human-in-the-Loop 태그 검사
  reqcov   <project_dir>                     요구사항 커버리지(요구사항 ↔ 시스템 테스트) 검사

종료 코드: 0 = 통과, 1 = 기준 미달, 2 = 사용법/입력 오류
오류 출력은 gcc 형식(`파일:줄: error: 메시지`)이라 Jenkins Warnings NG가 그대로 읽는다.
"""
import argparse
import json
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

EXIT_OK = 0
EXIT_FAIL = 1
EXIT_USAGE = 2

HITL_TAGS = ("ai_model", "ai_date", "requested_by",
             "human_reviewed", "reviewed_by", "reviewed_date")
DATE_RE = re.compile(r"^\d{4}-\d{2}-\d{2}([ T]\d{2}:\d{2}(:\d{2})?)?$")
CODE_SUFFIXES = {".h", ".hpp", ".hh", ".cpp", ".cc", ".cxx"}
DOC_BLOCK_RE = re.compile(r"/\*\*(?![*<])(.*?)\*/", re.S)


def cmd_tests(args):
    """GoogleTest JUnit XML에서 성공률과 건너뜀/비활성 개수를 검사한다."""
    result = EXIT_OK
    try:
        root = ET.parse(args.xml).getroot()
    except (OSError, ET.ParseError) as exc:
        print(f"{args.xml}: error: 시험 결과를 읽을 수 없음 ({exc})")
        result = EXIT_FAIL
    else:
        total = int(root.get("tests", "0"))
        failures = int(root.get("failures", "0"))
        errors = int(root.get("errors", "0"))
        disabled = int(root.get("disabled", "0"))
        skipped = sum(int(s.get("skipped", "0")) for s in root.iter("testsuite"))
        skipped += sum(1 for _ in root.iter("skipped"))
        passed = total - failures - errors
        rate = (100.0 * passed / total) if total else 0.0
        print(f"시험 {total}개: 성공 {passed}, 실패 {failures}, 오류 {errors}, "
              f"건너뜀 {skipped}, 비활성 {disabled} -> 성공률 {rate:.2f}%")
        if total == 0:
            print(f"{args.xml}: error: 실행된 시험이 없음")
            result = EXIT_FAIL
        elif failures or errors or skipped or disabled:
            print(f"{args.xml}: error: 성공률 100% 미달 (실패/오류/건너뜀/비활성 존재)")
            result = EXIT_FAIL
    return result


def cmd_coverage(args):
    """llvm-cov export -summary-only JSON을 요약하고 임계값을 검사한다."""
    result = EXIT_OK
    try:
        totals = json.loads(Path(args.json).read_text(encoding="utf-8"))["data"][0]["totals"]
    except (OSError, ValueError, KeyError, IndexError) as exc:
        print(f"{args.json}: error: 커버리지 요약을 읽을 수 없음 ({exc})")
        return EXIT_FAIL

    print(f"{'항목':<10}{'covered':>9}{'count':>9}{'percent':>10}")
    for metric in ("lines", "functions", "regions", "branches", "mcdc"):
        item = totals.get(metric)
        if item is None:
            print(f"{metric:<10}{'-':>9}{'-':>9}{'n/a':>10}")
        else:
            print(f"{metric:<10}{item['covered']:>9}{item['count']:>9}{item['percent']:>9.2f}%")

    for requirement in args.require:
        metric, _, threshold_text = requirement.partition("=")
        threshold = float(threshold_text)
        item = totals.get(metric)
        if item is None:
            print(f"{args.json}: error: {metric} 커버리지 정보 없음 (계측 옵션 확인)")
            result = EXIT_FAIL
        elif item["count"] == 0:
            print(f"{metric} 대상 없음(count=0) -> 해당 없음으로 통과")
        elif item["percent"] + 1e-9 < threshold:
            print(f"{args.json}: error: {metric} 커버리지 {item['percent']:.2f}% < 기준 {threshold:g}%")
            result = EXIT_FAIL
        else:
            print(f"{metric} 커버리지 {item['percent']:.2f}% >= 기준 {threshold:g}% 통과")
    return result


def _line_of(text, offset):
    return text.count("\n", 0, offset) + 1


def _tag_value(block, tag):
    match = re.search(r"[@\\]" + tag + r"[ \t]+(\S[^\n]*)", block)
    return match.group(1).strip() if match else None


def _check_block(path, text, match):
    """Doxygen 주석 한 블록의 HITL 태그를 검사하고 오류 문자열 목록을 돌려준다."""
    errors = []
    line = _line_of(text, match.start())
    block = match.group(1)
    values = {tag: _tag_value(block, tag) for tag in HITL_TAGS}
    missing = [tag for tag, value in values.items() if value is None]
    if missing:
        errors.append(f"{path}:{line}: error: HITL 태그 누락: " + ", ".join("@" + t for t in missing))
    else:
        reviewed = values["human_reviewed"].lower()
        if reviewed not in ("yes", "no"):
            errors.append(f"{path}:{line}: error: @human_reviewed 값은 yes/no 여야 함")
        if not DATE_RE.match(values["ai_date"]):
            errors.append(f"{path}:{line}: error: @ai_date 형식 오류 (YYYY-MM-DD[ HH:MM])")
        if reviewed == "yes":
            if values["reviewed_by"] == "-" or not DATE_RE.match(values["reviewed_date"]):
                errors.append(f"{path}:{line}: error: 리뷰 완료(yes)인데 @reviewed_by/@reviewed_date 가 유효하지 않음")
        elif reviewed == "no":
            if values["reviewed_by"] != "-" or values["reviewed_date"] != "-":
                errors.append(f"{path}:{line}: error: 리뷰 미완료(no)인데 @reviewed_by/@reviewed_date 가 '-' 가 아님")
    return errors


def cmd_hitl(args):
    """모든 소스/헤더의 /** */ 블록에 HITL 태그 6종이 있는지, 파일 헤더(@file)가 있는지 검사한다."""
    project = Path(args.project)
    errors = []
    checked_files = 0
    checked_blocks = 0
    for directory in args.dirs:
        for path in sorted((project / directory).rglob("*")):
            if path.suffix not in CODE_SUFFIXES or not path.is_file():
                continue
            if "build" in path.relative_to(project).parts:
                continue
            checked_files += 1
            text = path.read_text(encoding="utf-8", errors="replace")
            rel = path.relative_to(project).as_posix()
            blocks = list(DOC_BLOCK_RE.finditer(text))
            if not any(re.search(r"[@\\]file\b", m.group(1)) for m in blocks):
                errors.append(f"{rel}:1: error: 파일 헤더 주석(@file) 없음")
            for match in blocks:
                checked_blocks += 1
                errors.extend(_check_block(rel, text, match))
    for error in errors:
        print(error)
    print(f"HITL 검사: 파일 {checked_files}개, 주석 블록 {checked_blocks}개, 오류 {len(errors)}건")
    return EXIT_FAIL if errors else EXIT_OK


TABLE_ROW_RE = re.compile(r"^\|\s*(REQ-[FQLCS]-\d+)\s*\|(.*)$")


def _requirement_ids(path):
    ids = []
    if path.is_file():
        for line in path.read_text(encoding="utf-8").splitlines():
            match = TABLE_ROW_RE.match(line)
            if match:
                ids.append(match.group(1))
    return ids


def cmd_reqcov(args):
    """모든 요구사항 행이 추적 매트릭스에서 시스템 테스트(ST-)에 연결되는지 검사한다."""
    project = Path(args.project)
    requirements = _requirement_ids(project / "docs/requirements/software-requirements.md")
    requirements += _requirement_ids(project / "docs/safety/safety-requirements.md")
    result = EXIT_OK
    if not requirements:
        print("요구사항 문서에 요구사항이 없음 -> 해당 없음으로 통과")
    else:
        matrix = project / "docs/traceability/trace-matrix.md"
        if not matrix.is_file():
            print(f"{matrix}: error: 추적 매트릭스 없음")
            result = EXIT_FAIL
        else:
            linked = set()
            for line in matrix.read_text(encoding="utf-8").splitlines():
                match = TABLE_ROW_RE.match(line)
                if match and re.search(r"\bST-[\w.-]+", match.group(2)):
                    linked.add(match.group(1))
            missing = [rid for rid in requirements if rid not in linked]
            covered = len(requirements) - len(missing)
            print(f"요구사항 커버리지: {covered}/{len(requirements)} "
                  f"({100.0 * covered / len(requirements):.2f}%)")
            for rid in missing:
                print(f"{matrix}:1: error: {rid} 에 연결된 시스템 테스트(ST-) 없음")
            if missing:
                result = EXIT_FAIL
    return result


def build_parser():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)

    p_tests = sub.add_parser("tests")
    p_tests.add_argument("xml")
    p_tests.set_defaults(func=cmd_tests)

    p_cov = sub.add_parser("coverage")
    p_cov.add_argument("json")
    p_cov.add_argument("--require", action="append", default=[], metavar="METRIC=PERCENT")
    p_cov.set_defaults(func=cmd_coverage)

    p_hitl = sub.add_parser("hitl")
    p_hitl.add_argument("project")
    p_hitl.add_argument("dirs", nargs="+")
    p_hitl.set_defaults(func=cmd_hitl)

    p_req = sub.add_parser("reqcov")
    p_req.add_argument("project")
    p_req.set_defaults(func=cmd_reqcov)
    return parser


def main(argv):
    args = build_parser().parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
