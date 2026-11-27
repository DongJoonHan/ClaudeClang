#!/usr/bin/env python3
"""시스템 테스트 명세(JSON)와 실행 시험(test/system/*.cpp)을 한 데이터에서 만든다.

작성 정보: claude-sonnet-5-5, 2026-11-27, 요청자 DongJoonHan, 사용자 리뷰: 미완료.

사용:  python docs/test/system/gen_system_tests.py
산출:  docs/test/system/system-test-spec.json   (build_workbook.py 입력)
       test/system/ecl_system_test.cpp           (이터레이션 1 에서 실행 가능한 케이스)
       docs/test/system/system-test-cases.md      (케이스 목록과 실행 상태, 추적 매트릭스용)
기대값은 SRS(docs/requirements, docs/safety)의 규칙에서 직접 계산한다 (구현 출력을 베끼지 않는다).
"""
import json
import re
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
HITL = (" * @ai_model        claude-sonnet-5-5\n * @ai_date         2026-11-27\n"
        " * @requested_by    DongJoonHan\n * @human_reviewed  no\n * @reviewed_by     -\n * @reviewed_date   -\n")
ENV_SIL = "SIL: ecl_core 호스트 빌드(Clang, C17), test/system 시험이 controller 인터페이스(ARCH-I-006)로 실행"
ITER_RE = re.compile(r"^\| (REQ-[FQLCS]-\d+) \|.*?\| (\d|-) \| ARCH", re.M)

# ------------------------------------------------------------------ 베이시스 (SRS 에서 읽는다)
def read_basis():
    items = []
    for rel in ("docs/requirements/software-requirements.md", "docs/safety/safety-requirements.md"):
        for line in (REPO / rel).read_text(encoding="utf-8").splitlines():
            m = re.match(r"^\|\s*(REQ-([FQLCS])-\d+)\s*\|(.*)$", line)
            if not m:
                continue
            cells = [c.strip() for c in m.group(3).split("|")]
            text = max(cells, key=len)
            kind = "기능" if m.group(2) in "FS" else "비기능"
            if m.group(1) in ("REQ-S-001", "REQ-S-003"):
                kind = "비기능"
            items.append({"id": m.group(1), "title": text[:90], "type": kind,
                          "asil": "B" if m.group(2) == "S" else "QM"})
    iters = {}
    matrix = (REPO / "docs/traceability/trace-matrix.md").read_text(encoding="utf-8")
    for line in matrix.splitlines():
        cells = [c.strip() for c in line.split("|")]
        if len(cells) > 5 and cells[1].startswith("REQ-"):
            iters[cells[1]] = cells[4]
    return items, iters


TCS = []


def tc(id_, basis, purpose, inputs, procedure, expected, techs, pre=None, proposed=False, pict=None,
       cat="기능", exec_=None, env=ENV_SIL):
    TCS.append(dict(id=id_, category=cat, basis_ids=basis, purpose=purpose, preconditions=pre or [],
                    environment=env, inputs=inputs, procedure=procedure, expected=expected,
                    techniques=techs, proposed=proposed, pict_model=pict, exec=exec_))


F = [0]
NF = [0]


def fid():
    F[0] += 1
    return f"ST-F-{F[0]:03d}"


def nfid():
    NF[0] += 1
    return f"ST-NF-{NF[0]:03d}"


LOCK_L = ["ctl.confirmed.left = ECL_OUT_LOCK;"]   # 이전 확정 출력: 좌 LOCK, 우 RELEASE
PRE_PRIOR = ["이전 확정 출력: 좌 LOCK, 우 RELEASE", "모든 입력 정상(속도 10 km/h, 기어 D, 점화 켜짐)"]
HELD = ["EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);", "EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);"]


def step(setup=None, eval_=1.01, cmds=None):
    return dict(setup=setup or [], eval=eval_, cmds=cmds or [])


def simple(prior, setup, expect, eval_=1.01, cmds=None):
    return dict(prior=prior, steps=[step(setup, eval_, cmds)], expect=expect)


DEG = ["EXPECT_EQ(out.state, ECL_STATE_DEGRADED);", "EXPECT_EQ(out.warnings, ECL_WARN_INPUT_INVALID);",
       "EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_INVALID);",
       "EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);"] + HELD

# ------------------------------------------------------------------ 입력 검증 (REQ-F-001~005, REQ-S-008)
tc(fid(), ["REQ-F-001", "REQ-F-002", "REQ-F-004", "REQ-F-005", "REQ-F-016", "REQ-F-011"],
   "정상 입력(유효 클래스 대표값)이 ACTIVE 로 처리되고 이전 확정 출력이 유지됨을 확인한다.",
   {"vehicle_speed_kph": "10.0", "gear": "D", "crash_status": "NONE", "불리언 9종": "ignition_on=TRUE, 나머지 FALSE", "timestamp": "1.0 s, 평가 시각 1.01 s"},
   ["이전 확정 출력을 좌 LOCK, 우 RELEASE 로 만든다", "정상 입력으로 1주기를 실행한다", "상태와 출력을 관측한다"],
   "상태 ACTIVE, 경고 없음, 좌 LOCK, 우 RELEASE (규칙 없음 → HOLD, 이유 NONE)", ["동치 분할"], PRE_PRIOR,
   exec_=simple(LOCK_L, [], ["EXPECT_EQ(out.state, ECL_STATE_ACTIVE);", "EXPECT_EQ(out.warnings, 0U);",
                              "EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_HOLD);",
                              "EXPECT_EQ(out.decision.reason_code, ECL_REASON_NONE);"] + HELD))

for speed, label in ((0.0, "하한"), (300.0, "상한")):
    tc(fid(), ["REQ-F-001"], f"속도 {label} 경계값 {speed} km/h 가 유효임을 확인한다.", {"vehicle_speed_kph": f"{speed}"},
       ["속도만 바꿔 1주기를 실행한다"], "상태 ACTIVE, 경고 없음", ["경계값 분석"], PRE_PRIOR,
       exec_=simple(LOCK_L, [f"raw.speed_kph = Num({speed});"], ["EXPECT_EQ(out.state, ECL_STATE_ACTIVE);", "EXPECT_EQ(out.warnings, 0U);"]))

for speed, label, cpp in ((-0.1, "하한 미만", "-0.1"), (300.1, "상한 초과", "300.1"), ("누락", "누락", None), ("NaN", "NaN", "__builtin_nan(\"\")")):
    setup = ["raw.speed_kph = MissingNum();"] if cpp is None else [f"raw.speed_kph = Num({cpp});"]
    tc(fid(), ["REQ-F-001", "REQ-S-008"], f"속도 {label}({speed}) 는 INVALID 이며 제어 평가에 쓰이지 않음을 확인한다.",
       {"vehicle_speed_kph": str(speed)}, ["속도만 무효로 바꿔 1주기를 실행한다"],
       "상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지(좌 LOCK, 우 RELEASE)", ["동치 분할", "경계값 분석"], PRE_PRIOR,
       exec_=simple(LOCK_L, setup, DEG))

for code, label in ((0, "P"), (3, "R")):
    tc(fid(), ["REQ-F-002"], f"기어 코드 {code}({label}) 가 유효임을 확인한다.", {"gear": label},
       ["기어만 바꿔 1주기를 실행한다"], "상태 ACTIVE", ["경계값 분석"], PRE_PRIOR,
       exec_=simple(LOCK_L, [f"raw.gear = Int({code});"], ["EXPECT_EQ(out.state, ECL_STATE_ACTIVE);"]))
for val, label, cpp in (("미정 코드 4", "범위 밖", "Int(4)"), ("누락", "누락", "MissingInt()")):
    tc(fid(), ["REQ-F-002", "REQ-S-008"], f"기어 {label} 는 INVALID 이다.", {"gear": val}, ["기어만 무효로 바꿔 1주기를 실행한다"],
       "상태 DEGRADED, 경고 INPUT_INVALID, 출력 유지", ["동치 분할", "경계값 분석"], PRE_PRIOR,
       exec_=simple(LOCK_L, [f"raw.gear = {cpp};"], DEG))

for val, label, cpp in (("누락", "누락", "MissingNum()"), ("NaN", "NaN", "Num(__builtin_nan(\"\"))")):
    tc(fid(), ["REQ-F-003", "REQ-S-003", "REQ-S-008"], f"timestamp {label} 이면 입력 묶음 전체가 INVALID 이다 (신선도 판단 불가).",
       {"source_timestamp_s": val}, ["timestamp 만 무효로 바꿔 1주기를 실행한다"],
       "상태 DEGRADED, 경고 INPUT_INVALID(stale 아님), 출력 유지", ["동치 분할"], PRE_PRIOR,
       exec_=simple(LOCK_L, [f"raw.timestamp_s = {cpp};"], DEG))

tc(fid(), ["REQ-F-004"], "충돌 상태 NONE 이 유효하고 다른 영향이 없음을 확인한다 (PENDING/CONFIRMED 는 REQ-F-029, REQ-S-001 케이스).",
   {"crash_status": "NONE"}, ["충돌 상태 NONE 으로 1주기를 실행한다"], "상태 ACTIVE", ["동치 분할"], PRE_PRIOR,
   exec_=simple(LOCK_L, ["raw.crash_status = Int(0);"], ["EXPECT_EQ(out.state, ECL_STATE_ACTIVE);"]))
for val, label, cpp in (("미정 코드 3", "범위 밖", "Int(3)"), ("누락", "누락", "MissingInt()")):
    tc(fid(), ["REQ-F-004", "REQ-S-008"], f"충돌 상태 {label} 는 INVALID 이다.", {"crash_status": val},
       ["충돌 상태만 무효로 바꿔 1주기를 실행한다"], "상태 DEGRADED, 경고 INPUT_INVALID, 출력 유지", ["동치 분할", "경계값 분석"], PRE_PRIOR,
       exec_=simple(LOCK_L, [f"raw.crash_status = {cpp};"], DEG))

BOOLS = [("rear_left_risk", "rear_left_risk"), ("rear_right_risk", "rear_right_risk"), ("fire_detected", "fire"),
         ("overtemperature_detected", "overtemp"), ("adult_present", "adult_present"), ("isofix_left", "isofix_left"),
         ("isofix_right", "isofix_right"), ("ignition_on", "ignition_on"), ("sensor_fault", "sensor_fault")]
for name, member in BOOLS:
    for val, label, cpp in (("2", "불리언 아님(2)", "Int(2)"), ("누락", "누락", "MissingInt()")):
        tc(fid(), ["REQ-F-005", "REQ-S-008"], f"{name} {label} 는 INVALID 이며 그 입력에 의존하는 규칙은 적용되지 않는다.",
           {name: val}, [f"{name} 만 무효로 바꿔 1주기를 실행한다"],
           "상태 DEGRADED, 경고 INPUT_INVALID, 이유 INPUT_INVALID, 출력 유지 (FAULT/RELEASE/LOCK 규칙 미적용)", ["동치 분할"], PRE_PRIOR,
           exec_=simple(LOCK_L, [f"raw.{member} = {cpp};"], DEG))

# ------------------------------------------------------------------ 운전자 명령 (REQ-F-006~009, 018, 019)
CMD_HELD = ["EXPECT_EQ(out.decision.left, ECL_OUT_LOCK);", "EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);"]
for fld, req, vals in (("side", "REQ-F-006", ("3", "누락")), ("action", "REQ-F-018", ("2", "누락")), ("source", "REQ-F-019", ("4", "누락"))):
    for val in vals:
        cmd = f"[&] {{ ecl_raw_command_t c = ValidCommand(); c.{fld} = " + ("MissingInt()" if val == "누락" else f"Int({val})") + "; return c; }()"
        tc(fid(), [req, "REQ-F-007"], f"운전자 명령의 {fld} 가 {'누락' if val == '누락' else '범위 밖 ' + val} 이면 명령을 거절하고 평가에 쓰지 않는다.",
           {f"command.{fld}": val, "나머지 명령 필드": "유효"}, ["명령 1건과 함께 1주기를 실행한다", "명령 거절 이유와 출력을 관측한다"],
           "command_note = CMD_REJECTED_INVALID, 출력 변화 없음(좌 LOCK, 우 RELEASE)", ["동치 분할", "경계값 분석"], PRE_PRIOR,
           exec_=simple(LOCK_L, [], ["EXPECT_EQ(out.decision.command_note, ECL_REASON_CMD_REJECTED_INVALID);"] + CMD_HELD, cmds=[cmd]))
tc(fid(), ["REQ-F-009", "REQ-F-007"], "한 평가주기에 유효한 명령 2건이 오면 첫 1건만 평가에 쓰고 나머지는 거절 이유로 기록한다.",
   {"commands": "좌 잠금(물리 버튼), 우 해제(AVN)"}, ["명령 2건과 함께 1주기를 실행한다"],
   "command_note = CMD_REJECTED_EXTRA", ["동치 분할", "WoW 검증"], PRE_PRIOR,
   exec_=simple(LOCK_L, [], ["EXPECT_EQ(out.decision.command_note, ECL_REASON_CMD_REJECTED_EXTRA);"],
                cmds=["Cmd(ECL_SIDE_LEFT, ECL_ACTION_LOCK, ECL_SRC_PHYSICAL_BUTTON)", "Cmd(ECL_SIDE_RIGHT, ECL_ACTION_UNLOCK, ECL_SRC_AVN)"]))
tc(fid(), ["REQ-F-008", "REQ-F-023"], "운전자 명령의 요청 시각이 입력 묶음의 source_timestamp_s 임을 override 10초 경계로 확인한다 (평가 시각이 달라도 timestamp 기준).",
   {"접근위험": "TRUE(좌)", "해제 명령 1": "timestamp 100.0 s, 평가 시각 100.0 s", "해제 명령 2": "timestamp 109.9 s, 평가 시각 130.0 s(지연 평가)"},
   ["좌 접근위험 TRUE 로 첫 해제 명령을 보낸다 (억제됨)", "timestamp 109.9, 평가 시각 130.0 으로 같은 해제 명령을 다시 보낸다"],
   "두 번째 명령이 override 로 처리된다 (timestamp 차 9.9 s ≤ 10 s). 평가 시각 차(30 s)를 쓰면 override 되지 않으므로 구분된다.",
   ["경계값 분석", "WoW 검증"], ["이터레이션 3 이후 실행"])

# ------------------------------------------------------------------ 상태 (REQ-F-014, REQ-S-003/004/009)
tc(fid(), ["REQ-F-014", "REQ-S-004", "REQ-S-009", "REQ-F-010", "REQ-F-011"], "sensor_fault 가 유효한 TRUE 이면 첫 주기에 FAULT, 경고, 출력 유지, 규칙 1 이 적용된다.",
   {"sensor_fault": "TRUE", "이전 확정 출력": "좌 LOCK, 우 RELEASE"}, ["sensor_fault 를 TRUE 로 1주기를 실행한다"],
   "상태 FAULT, 경고 SENSOR_FAULT, 좌 LOCK, 우 RELEASE, priority SENSOR_FAULT, reason SENSOR_FAULT", ["동치 분할"], PRE_PRIOR,
   exec_=simple(LOCK_L, ["raw.sensor_fault = Int(1);"],
                ["EXPECT_EQ(out.state, ECL_STATE_FAULT);", "EXPECT_EQ(out.warnings, ECL_WARN_SENSOR_FAULT);",
                 "EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_SENSOR_FAULT);",
                 "EXPECT_EQ(out.decision.reason_code, ECL_REASON_SENSOR_FAULT);"] + HELD))
tc(fid(), ["REQ-F-010", "REQ-S-004", "REQ-S-008"], "sensor_fault 가 유효한 TRUE 이고 다른 필드(속도)가 INVALID 이어도 규칙 1 은 자신의 입력이 유효하므로 적용된다.",
   {"sensor_fault": "TRUE", "vehicle_speed_kph": "500 (범위 밖)"}, ["두 값을 바꿔 1주기를 실행한다"],
   "상태 FAULT, 경고 SENSOR_FAULT|INPUT_INVALID, 출력 유지, priority SENSOR_FAULT", ["동치 분할", "WoW 검증"], PRE_PRIOR,
   exec_=simple(LOCK_L, ["raw.sensor_fault = Int(1);", "raw.speed_kph = Num(500.0);"],
                ["EXPECT_EQ(out.state, ECL_STATE_FAULT);", "EXPECT_EQ(out.warnings, ECL_WARN_SENSOR_FAULT | ECL_WARN_INPUT_INVALID);",
                 "EXPECT_EQ(out.decision.priority_reason, ECL_PRIO_SENSOR_FAULT);"] + HELD))
tc(fid(), ["REQ-F-010", "REQ-F-014"], "sensor_fault 입력이 INVALID 이면 규칙 1 은 적용되지 않고 FAULT 가 아니다.",
   {"sensor_fault": "2 (불리언 아님)"}, ["sensor_fault 만 무효로 1주기를 실행한다"], "상태 DEGRADED (FAULT 아님), priority HOLD",
   ["동치 분할"], PRE_PRIOR, exec_=simple(LOCK_L, ["raw.sensor_fault = Int(2);"], DEG))
tc(fid(), ["REQ-F-014", "REQ-S-003"], "stale 입력(나이 0.25 s)은 DEGRADED 이고 INPUT_STALE 경고, 이유 INPUT_STALE, 출력 유지이다.",
   {"timestamp": "1.0 s", "평가 시각": "1.25 s"}, ["평가 시각만 1.25 로 1주기를 실행한다"],
   "상태 DEGRADED, 경고 INPUT_STALE, 이유 INPUT_STALE, 출력 유지", ["경계값 분석"], PRE_PRIOR,
   exec_=simple(LOCK_L, [], ["EXPECT_EQ(out.state, ECL_STATE_DEGRADED);", "EXPECT_EQ(out.warnings, ECL_WARN_INPUT_STALE);",
                              "EXPECT_EQ(out.decision.reason_code, ECL_REASON_INPUT_STALE);"] + HELD, eval_=1.25))
tc(fid(), ["REQ-F-014"], "점화가 꺼지면(유효) 상태가 OFF 이다 (이전 출력이 RELEASE 인 경우로 출력과 분리해 확인).",
   {"ignition_on": "FALSE"}, ["이전 출력 좌/우 RELEASE 에서 ignition_on=FALSE 로 1주기를 실행한다"], "상태 OFF, 경고 없음", ["동치 분할"],
   ["이전 확정 출력: 좌/우 RELEASE"], exec_=simple([], ["raw.ignition_on = Int(0);"], ["EXPECT_EQ(out.state, ECL_STATE_OFF);", "EXPECT_EQ(out.warnings, 0U);"]))
tc(fid(), ["REQ-F-014", "REQ-S-009"], "상태 우선순서: sensor_fault(FAULT) > 입력 오류(DEGRADED) > OFF.", {"조합": "점화 FALSE + 속도 500 → DEGRADED, 점화 FALSE + sensor_fault TRUE → FAULT"},
   ["점화 FALSE 와 속도 500 으로 1주기", "점화 FALSE 와 sensor_fault TRUE 로 1주기"], "첫째 DEGRADED, 둘째 FAULT", ["상태 전이"], ["이전 확정 출력: 좌/우 RELEASE"],
   exec_=dict(prior=[], steps=[step(["raw.ignition_on = Int(0);", "raw.speed_kph = Num(500.0);"], 1.01), step(["raw.ignition_on = Int(0);", "raw.sensor_fault = Int(1);"], 1.02)],
              expect=["EXPECT_EQ(out.state, ECL_STATE_FAULT);"],
              extra=[("after_step_0", ["EXPECT_EQ(out.state, ECL_STATE_DEGRADED);"])]))
tc(fid(), ["REQ-F-014", "REQ-F-010"], "오류가 해소되면 다음 평가주기에 자동으로 ACTIVE 로 복귀한다 (DEGRADED 와 FAULT 모두).",
   {"주기 1": "속도 500", "주기 2": "정상", "주기 3": "sensor_fault TRUE", "주기 4": "정상"}, ["4주기를 순서대로 실행한다"],
   "주기 1 DEGRADED, 2 ACTIVE(이유 NONE), 3 FAULT, 4 ACTIVE(이유 NONE)", ["상태 전이"], PRE_PRIOR,
   exec_=dict(prior=LOCK_L, steps=[step(["raw.speed_kph = Num(500.0);"], 1.01), step([], 1.02), step(["raw.sensor_fault = Int(1);"], 1.03), step([], 1.04)],
              expect=["EXPECT_EQ(out.state, ECL_STATE_ACTIVE);", "EXPECT_EQ(out.decision.reason_code, ECL_REASON_NONE);"] + HELD,
              extra=[("after_step_0", ["EXPECT_EQ(out.state, ECL_STATE_DEGRADED);"]), ("after_step_1", ["EXPECT_EQ(out.state, ECL_STATE_ACTIVE);"]),
                     ("after_step_2", ["EXPECT_EQ(out.state, ECL_STATE_FAULT);"])]))

# ------------------------------------------------------------------ 주기/초기화/출력/독립 (REQ-F-013, 015, 016, 017)
tc(fid(), ["REQ-F-015", "REQ-F-016"], "전원 투입 직후 첫 평가주기의 출력은 좌/우 RELEASE 이고 값이 LOCK 또는 RELEASE 로 공개된다.",
   {"입력": "정상"}, ["제어기를 초기화한 직후 1주기를 실행한다"], "좌 RELEASE, 우 RELEASE, 상태 ACTIVE", ["동치 분할"], ["초기화 직후"],
   exec_=simple([], [], ["EXPECT_EQ(out.decision.left, ECL_OUT_RELEASE);", "EXPECT_EQ(out.decision.right, ECL_OUT_RELEASE);", "EXPECT_EQ(out.state, ECL_STATE_ACTIVE);"]))
tc(fid(), ["REQ-F-013", "REQ-F-016"], "10 ms 평가주기로 100주기를 실행하고, 오류 입력이 같은 주기에 상태에 반영됨(지연 없음)을 확인한다.",
   {"평가 시각": "1.00 + 0.01 k s (k=0..99)", "입력": "홀수 k 에서 속도 500"}, ["100주기를 실행하며 매 주기 상태를 관측한다"],
   "짝수 k: ACTIVE, 홀수 k: DEGRADED (입력 오류가 같은 주기에 반영)", ["상태 전이", "WoW 검증"], PRE_PRIOR,
   exec_=dict(body="""ecl_controller_t ctl{};
ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
for (int k = 0; k < 100; ++k)
{
    ecl_raw_snapshot_t raw = ValidRaw();
    const double now = 1.0 + (0.01 * k);
    raw.timestamp_s = Num(now);
    if ((k % 2) == 1)
    {
        raw.speed_kph = Num(500.0);
    }
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, now, &out), ECL_OK);
    EXPECT_EQ(out.state, ((k % 2) == 1) ? ECL_STATE_DEGRADED : ECL_STATE_ACTIVE) << "k=" << k;
}"""))
tc(fid(), ["REQ-F-017"], "좌/우 출력이 독립적으로 결정됨을 확인한다 (한쪽 규칙이 반대쪽 출력을 바꾸지 않음).", {"접근위험(좌)": "TRUE", "이전 확정 출력": "좌 RELEASE, 우 LOCK"},
   ["좌만 접근위험 TRUE 로 1주기를 실행한다"], "좌 LOCK(규칙 3), 우 LOCK 유지(규칙 없음)", ["동치 분할"], ["이전 확정 출력: 좌 RELEASE, 우 LOCK"] + [""][:0] + ["이터레이션 2 이후 실행"])

# ------------------------------------------------------------------ 규칙 (REQ-S-001/002/005/006/007, REQ-F-012/020, PICT)
tc(fid(), ["REQ-S-001", "REQ-S-005"], "충돌 CONFIRMED 가 유효하면 300 ms 이내(첫 평가주기 이후 ≤ 30 주기)에 좌/우 RELEASE 가 되고 CONFIRMED 인 동안 유지된다.",
   {"crash_status": "CONFIRMED (t0 부터 3 s 유지)", "이전 확정 출력": "좌 LOCK, 우 LOCK"}, ["이전 출력을 좌/우 LOCK 으로 한다", "t0 에 CONFIRMED 를 주입하고 10 ms 주기로 300 주기를 실행한다", "RELEASE 가 된 첫 시각과 이후 출력을 관측한다"],
   "RELEASE 가 t0 부터 300 ms 이내에 되고, 이후 300주기 동안 계속 RELEASE", ["경계값 분석", "성능 측정"], ["이터레이션 2 이후 실행"], cat="비기능")
for t_, label in (("직전 상태 NONE → CONFIRMED 전이", "전이"), ("PENDING → CONFIRMED 전이", "PENDING 경유")):
    tc(fid(), ["REQ-S-001"], f"충돌 {label} 에서 RELEASE 가 된다.", {"crash_status": t_}, ["전이를 입력한다", "다음 평가주기 출력을 관측한다"],
       "좌/우 RELEASE, priority CRASH, reason CRASH_CONFIRMED", ["상태 전이"], ["이터레이션 2 이후 실행"])
tc(fid(), ["REQ-F-029"], "충돌 PENDING 은 출력을 바꾸지 않고 CRASH_PENDING 만 기록하며 다른 규칙은 영향받지 않는다.", {"crash_status": "PENDING", "추가": "속도 10 km/h"},
   ["PENDING 으로 1주기", "PENDING + 속도 3.0 km/h 로 1주기"], "첫째: 출력 유지, 이유 CRASH_PENDING / 둘째: 규칙 7 에 의해 LOCK (PENDING 이 막지 않음)", ["동치 분할", "WoW 검증"], ["이터레이션 2 이후 실행"])
tc(fid(), ["REQ-S-002", "REQ-S-006", "REQ-S-007", "REQ-F-017"], "좌 접근위험 TRUE 이면 좌 LOCK, 좌 해제 요청 억제, 억제 이유 기록, 우는 영향 없음.",
   {"rear_left_risk": "TRUE", "명령": "좌 해제(AVN), 정지 상태", "이전 확정 출력": "좌/우 RELEASE"}, ["명령과 함께 1주기를 실행한다"],
   "좌 LOCK, 우 RELEASE, 이유 APPROACH_RISK_HOLD, priority APPROACH", ["동치 분할"], ["이터레이션 2 이후 실행"])
tc(fid(), ["REQ-S-002"], "접근위험이 FALSE 로 돌아오면 억제가 풀리고 해제 명령이 적용된다.", {"주기 1": "좌 접근위험 TRUE", "주기 2": "접근위험 FALSE + 좌 해제 명령"},
   ["두 주기를 실행한다"], "주기 1 좌 LOCK, 주기 2 좌 RELEASE (규칙 8)", ["상태 전이"], ["이터레이션 2 이후 실행"])

PICT = json.loads((REPO / "docs/test/system/pict_PW-01.json").read_text(encoding="utf-8"))


def expected_for(row):
    """SRS 1.3절 규칙표에서 직접 계산한 기대 출력. 이전 확정 출력은 좌 LOCK, 우 RELEASE."""
    left, right = "LOCK", "RELEASE"
    prio = "HOLD"
    cmd_unlock_left = row["command"] == "left_unlock"
    sf = row["sensor_fault"] == "1"
    crash = row["crash_status"] == "CONFIRMED"
    approach = row["approach_left"] == "1"
    emerg = row["emergency"] != "none"
    ign_off = row["ignition_on"] == "0"
    iso = row["isofix_left"] == "1"
    moving = float(row["speed_kph"]) >= 3.0
    if sf:
        prio = "SENSOR_FAULT"
    elif crash:
        left, right, prio = "RELEASE", "RELEASE", "CRASH"
    else:
        applied = []
        l_rule = r_rule = None
        if approach:
            l_rule = ("LOCK", "APPROACH")
        if l_rule is None and emerg:
            l_rule = ("RELEASE", "EMERGENCY_RELEASE")
        if l_rule is None and ign_off:
            l_rule = ("RELEASE", "IGNITION_OFF")
        if l_rule is None and iso:
            l_rule = ("LOCK", "ISOFIX")
        if l_rule is None and moving:
            l_rule = ("LOCK", "AUTO_LOCK")
        if l_rule is None and cmd_unlock_left:
            l_rule = ("RELEASE", "DRIVER_CMD")
        if emerg:
            r_rule = ("RELEASE", "EMERGENCY_RELEASE")
        elif ign_off:
            r_rule = ("RELEASE", "IGNITION_OFF")
        elif moving:
            r_rule = ("LOCK", "AUTO_LOCK")
        order = ["APPROACH", "EMERGENCY_RELEASE", "IGNITION_OFF", "ISOFIX", "AUTO_LOCK", "DRIVER_CMD"]
        names = [x[1] for x in (l_rule, r_rule) if x]
        if l_rule:
            left = l_rule[0]
        if r_rule:
            right = r_rule[0]
        if names:
            prio = min(names, key=order.index)
    return left, right, prio


pict_ids = []
for n, row in enumerate(PICT["rows"], start=1):
    left, right, prio = expected_for(row)
    tid = fid()
    pict_ids.append(tid)
    tc(tid, ["REQ-F-012", "REQ-F-020"], f"PICT 조합 #{n}: 규칙 우선순위 상호작용(좌/우 독립 결정, 이전 확정 출력 좌 LOCK, 우 RELEASE).",
       {k: v for k, v in row.items()}, ["이전 확정 출력을 좌 LOCK, 우 RELEASE 로 한다", "입력 조합으로 1주기를 실행한다", "출력과 priority_reason 을 관측한다"],
       f"좌 {left}, 우 {right}, priority_reason {prio} (SRS 1.3절 규칙표에서 계산)", ["페어와이즈(PICT)"], ["이터레이션 2~3 이후 실행"], pict="PW-01")

# ------------------------------------------------------------------ 이터레이션 3 규칙 (REQ-F-021~027, 034~037)
tc(fid(), ["REQ-F-021", "REQ-F-034"], "정지 상태에서 네 입력 경로(source)와 side 3종이 선택한 도어에만 동작을 적용한다.", {"source": "physical_button, avn, voice, mobile_app", "side": "left, right, all", "action": "lock, unlock"},
   ["정지·정상 입력에서 조합마다 명령을 보낸다"], "side 가 가리키는 도어에만 action 적용, 네 source 의 결과가 같다", ["동치 분할"], ["이터레이션 3 이후 실행"])
tc(fid(), ["REQ-F-035"], "이동 중(속도 3.0 이상) 유효한 명령은 적용되지 않고 CMD_REJECTED_MOVING 으로 기록된다.", {"속도": "3.0 km/h", "명령": "좌 해제"},
   ["명령과 함께 1주기"], "좌 LOCK(규칙 7), command_note CMD_REJECTED_MOVING", ["경계값 분석"], ["이터레이션 3 이후 실행"])
for sp, exp, lab in ((2.9, "자동 잠금 아님(HOLD)", "직전"), (3.0, "좌/우 LOCK (규칙 7, 이유 AUTO_LOCK_MOVING)", "경계"), (3.1, "좌/우 LOCK", "직후")):
    tc(fid(), ["REQ-F-022"], f"속도 {sp} km/h ({lab}) 에서 자동 잠금 판정.", {"vehicle_speed_kph": str(sp)}, ["이전 출력 RELEASE 에서 1주기"], exp, ["경계값 분석"], ["이터레이션 3 이후 실행"])
for dt, exp in ((10.0, "override"), (10.1, "override 아님(첫 억제와 동일하게 LOCK 유지)")):
    tc(fid(), ["REQ-F-023", "REQ-F-036"], f"첫 억제 후 {dt} s 에 같은 해제 재입력.", {"간격": f"{dt} s"}, ["접근위험 TRUE 에서 해제 명령", f"{dt} s 후 같은 해제 명령"], exp + ", override 면 좌 RELEASE 와 reason OVERRIDE", ["경계값 분석"], ["이터레이션 3 이후 실행"])
for sig, reason in (("fire_detected", "FIRE"), ("overtemperature_detected", "OVERTEMP"), ("adult_present", "ADULT_PRESENT")):
    tc(fid(), ["REQ-F-025"], f"{sig} 단독 TRUE 이면 다음 주기에 좌/우 RELEASE.", {sig: "TRUE"}, ["이전 좌/우 LOCK 에서 신호 주입 후 1주기"], f"좌/우 RELEASE, reason {reason}", ["동치 분할"], ["이터레이션 3 이후 실행"])
for lab, left, right in (("isofix_left 만", "LOCK", "RELEASE"), ("isofix_right 만", "RELEASE", "LOCK")):
    tc(fid(), ["REQ-F-026", "REQ-F-017"], f"{lab} TRUE 이면 해당 도어만 LOCK, 반대쪽은 그대로.", {"입력": lab}, ["이전 좌/우 RELEASE 에서 1주기"], f"좌 {left}, 우 {right}", ["동치 분할"], ["이터레이션 3 이후 실행"])
tc(fid(), ["REQ-F-027"], "ignition_on 이 FALSE 로 바뀐 첫 주기에 좌/우 RELEASE, 상태 OFF, 이유 IGNITION_OFF.", {"ignition_on": "TRUE→FALSE"}, ["이전 좌/우 LOCK 에서 1주기"], "좌/우 RELEASE, OFF, reason IGNITION_OFF", ["상태 전이"], ["이터레이션 3 이후 실행"])
tc(fid(), ["REQ-F-028", "REQ-F-037"], "각 규칙이 결정한 출력의 reason_code 와 priority_reason 이 부록 A 의 대응 표와 일치한다.", {"규칙": "1~8, HOLD"}, ["규칙마다 단독 조건을 만들어 1주기씩 실행한다"],
   "priority_reason 과 reason_code 가 부록 A 의 대응과 같다", ["동치 분할"], ["이터레이션 2~3 이후 실행"])

# ------------------------------------------------------------------ 표시/이력 (REQ-F-024, 030~033, Q-001)
tc(fid(), ["REQ-F-024"], "조회 응답에 좌/우 출력, 상태, 입력별 유효성, 최근 이유코드가 포함된다.", {"조회": "1주기 실행 후"}, ["주기를 실행한 뒤 조회한다"], "응답에 네 항목이 모두 있고 값이 마지막 주기 결과와 같다", ["동치 분할"], ["이터레이션 4 이후 실행"])
tc(fid(), ["REQ-F-030", "REQ-F-038"], "결정 101건을 입력하면 최신 100건만 오래된 순으로 조회된다.", {"결정 수": "100, 101"}, ["결정 100건 후 조회", "1건 더 입력 후 조회"], "100건: 처음 100건 순서대로 / 101건: 첫 결정이 빠지고 2~101번째가 순서대로", ["경계값 분석"], ["이터레이션 4 이후 실행"])
tc(fid(), ["REQ-F-031"], "결정 이력 항목에 허용 필드(이벤트 ID, 시각, 좌/우 출력, 상태, reason_code)만 있다.", {"검사": "조회 응답 필드 목록"}, ["조회 응답의 필드를 열거한다"], "허용 필드만 존재", ["요구사항 분석"], ["이터레이션 4 이후 실행"])
tc(fid(), ["REQ-F-032"], "프로세스를 재기동하면 이력이 남지 않는다.", {"재기동": "1회"}, ["결정을 쌓은 뒤 프로세스를 재시작하고 조회한다"], "이력 0건", ["WoW 검증"], ["이터레이션 4 이후 실행"])
tc(fid(), ["REQ-F-033"], "표시 응답 직렬화 실패(버퍼 부족)는 HTTP 500 으로 응답된다.", {"버퍼 용량": "부족하게 설정"}, ["Web 시뮬레이터 경로에서 조회한다"], "HTTP 500", ["폴트 주입"], ["이터레이션 5 이후 실행"])

# ------------------------------------------------------------------ 안전 시간 요건 (REQ-S-003)
tc(nfid(), ["REQ-S-003", "REQ-F-014"], "입력이 200 ms 를 넘게 갱신되지 않으면 그 시점부터 100 ms 이내(≤ 10 주기)에 DEGRADED 가 된다. 신선한 동안은 ACTIVE 이다.",
   {"source_timestamp_s": "1.0 s 고정", "평가 시각": "1.00 + 0.01 k s (k=0..40)"}, ["동일 timestamp 로 10 ms 주기 41주기를 실행한다", "처음 DEGRADED 가 된 k 를 구한다"],
   "k ≤ 19 에서 ACTIVE, 처음 DEGRADED 는 k=21 이상 k=30 이하 (나이 > 0.2 s 후 100 ms 이내), 그 이후 계속 DEGRADED", ["경계값 분석", "성능 측정"], PRE_PRIOR, cat="비기능",
   exec_=dict(body="""ecl_controller_t ctl{};
ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
const ecl_raw_snapshot_t raw = ValidRaw();
int firstDegraded = -1;
for (int k = 0; k <= 40; ++k)
{
    ecl_cycle_result_t out{};
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.0 + (k / 100.0), &out), ECL_OK);
    if ((k <= 19) && (out.state != ECL_STATE_ACTIVE))
    {
        ADD_FAILURE() << "k=" << k << " 에서 ACTIVE 여야 한다";
    }
    if ((firstDegraded < 0) && (out.state == ECL_STATE_DEGRADED))
    {
        firstDegraded = k;
    }
    if (firstDegraded >= 0)
    {
        EXPECT_EQ(out.state, ECL_STATE_DEGRADED) << "k=" << k;
    }
}
EXPECT_GE(firstDegraded, 21);
EXPECT_LE(firstDegraded, 30);"""))
tc(nfid(), ["REQ-Q-003"], "평가주기 한 번의 평균 처리 시간이 3 ms 미만이다 (호스트, 계측 빌드, 10000주기 평균).", {"반복": "10000주기", "입력": "정상"},
   ["10000주기를 실행해 평균 처리 시간을 측정한다"], "평균 < 3 ms", ["성능 측정"], [], cat="비기능",
   exec_=dict(body="""ecl_controller_t ctl{};
ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
const ecl_raw_snapshot_t raw = ValidRaw();
ecl_cycle_result_t out{};
const int cycles = 10000;
const auto begin = std::chrono::steady_clock::now();
for (int i = 0; i < cycles; ++i)
{
    ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
}
const auto elapsed = std::chrono::steady_clock::now() - begin;
const double averageMs = std::chrono::duration<double, std::milli>(elapsed).count() / cycles;
EXPECT_LT(averageMs, 3.0);"""))
tc(nfid(), ["REQ-Q-001"], "같은 초기 상태·같은 입력 순서를 고정 시계로 1,000회 재생하면 공개 결과 해시가 모두 같다.", {"재생": "1,000회", "벡터": "정상/오류/stale/FAULT/OFF 혼합"},
   ["벡터를 재생해 공개 결과 해시를 구한다 (1,000회)"], "1,000개의 해시가 모두 같다", ["WoW 검증"], ["이터레이션 4 이후 실행 (해시 정의는 SRS 부록 B)"], cat="비기능")
tc(nfid(), ["REQ-Q-002", "REQ-Q-004"], "원시 입력 형식에 영상·음성·식별자 필드가 없고 성인 존재는 불리언 하나뿐이다.", {"검사": "ecl_raw_snapshot_t 구조 크기와 필드"},
   ["구조 크기를 허용 필드(수치 2개, 정수 코드 11개) 합과 비교한다", "adult_present 가 정수 코드 불리언 필드임을 확인한다"], "크기가 같고 adult_present 는 0/1 만 유효", ["요구사항 분석"], [], cat="비기능",
   exec_=dict(body="""const size_t expected = (2U * sizeof(ecl_raw_num_t)) + (11U * sizeof(ecl_raw_int_t));
EXPECT_EQ(sizeof(ecl_raw_snapshot_t), expected);
ecl_raw_snapshot_t raw = ValidRaw();
raw.adult_present = Int(2);
ecl_controller_t ctl{};
ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);
ecl_cycle_result_t out{};
ASSERT_EQ(ecl_controller_step(&ctl, &raw, nullptr, 0, 1.01, &out), ECL_OK);
EXPECT_EQ(out.state, ECL_STATE_DEGRADED);"""))

# ------------------------------------------------------------------ 제한/제약 (검사)
INSP = {
    "REQ-L-001": ("SW 소스에 래치/구동기/CAN·LIN 드라이버 코드가 없고 출력은 논리 값(LOCK/RELEASE)뿐임을 검사한다.", "src/, include/ 파일 목록과 공개 출력 형식 검사", "하드웨어 접근 코드 없음"),
    "REQ-L-002": ("출력 적용 feedback 입력이 인터페이스에 없음을 검사한다.", "ecl_raw_snapshot_t 필드 검사", "feedback 필드 없음"),
    "REQ-L-003": ("센서 진단·영상 처리·HMI 코드가 범위에 없음을 검사한다.", "src/ 파일 목록 검사", "해당 코드 없음"),
    "REQ-L-004": ("산출물이 인증·법규 적합성을 주장하지 않음을 검사한다.", "docs/ 와 PR 설명의 적합성 문구 검색", "적합성·인증 주장 문구 없음"),
    "REQ-C-001": ("제품 코드가 C17 로 Clang 18 이상으로 컴파일됨을 검사한다.", "CI 빌드 로그와 compile_commands.json 의 -std 옵션", "제품 .c 컴파일 옵션에 -std=c17, Clang 버전 ≥ 18"),
    "REQ-C-002": ("PC/SIL 호스트 빌드가 빌드 이미지에서 실행됨을 검사한다 (CR-001 OEM 승인 별도).", "ecl_sil 을 빌드 이미지에서 실행", "실행 성공"),
    "REQ-C-003": ("Web 시뮬레이터가 C 코어 결과를 표시만 하고 판단은 C 코어만 함을 검사한다.", "Web 시뮬레이터 소스에 규칙 판단 코드가 없는지 검사", "판단 로직 없음"),
    "REQ-C-004": ("산출물에 HARA/ASIL 결정 근거 재구성이 없음을 검사한다.", "docs/safety 검사", "ASIL 은 OEM 입력으로만 기재"),
    "REQ-C-005": ("CLAUDE.md 2절 품질 기준이 CI 에서 충족됨을 검사한다.", "ci/scripts/run-all.sh 결과", "모든 단계 통과"),
    "REQ-C-006": ("ISO 26262-6 준수·적합 주장이 없음을 검사한다.", "docs/ 문구 검색", "준수·적합 주장 없음"),
}
for rid, (purpose, how, exp) in INSP.items():
    tc(nfid(), [rid], purpose, {"검사 대상": how}, [how], exp, ["요구사항 분석"], [], cat="비기능",
       env="검사(inspection): 저장소·CI 산출물", proposed=False)

# ------------------------------------------------------------------ 출력
def cpp_for(tcase):
    ex = tcase["exec"]
    lines = []
    if "body" in ex:
        lines = ex["body"].splitlines()
    else:
        lines += ["ecl_controller_t ctl{};", "ASSERT_EQ(ecl_controller_init(&ctl), ECL_OK);"]
        lines += ex["prior"]
        lines += ["ecl_cycle_result_t out{};"]
        extras = dict(ex.get("extra", []))
        for i, st in enumerate(ex["steps"]):
            lines += ["{", "    ecl_raw_snapshot_t raw = ValidRaw();"]
            lines += ["    " + s for s in st["setup"]]
            if st["cmds"]:
                lines += [f"    const ecl_raw_command_t cmds[{len(st['cmds'])}] = {{" + ", ".join(st["cmds"]) + "};"]
                cmd_arg, count = "cmds", len(st["cmds"])
            else:
                cmd_arg, count = "nullptr", 0
            lines += [f"    ASSERT_EQ(ecl_controller_step(&ctl, &raw, {cmd_arg}, {count}, {st['eval']}, &out), ECL_OK);"]
            for e in extras.get(f"after_step_{i}", []):
                lines += ["    " + e]
            lines += ["}"]
        lines += ex["expect"]
    return lines


def main():
    basis, iters = read_basis()
    ids = {b["id"] for b in basis}
    for t in TCS:
        for b in t["basis_ids"]:
            assert b in ids, (t["id"], b)
    # 실행 상태: 베이시스의 구현 이터레이션 정보와 TC 의 exec 유무
    for t in TCS:
        runnable = t["exec"] is not None
        status = "자동 실행(이터레이션 1)" if runnable else ("검사 대기 (G3/G4 제출 전 수행, 이번 이터레이션 미수행)" if t["environment"].startswith("검사") else "미실시 (" + "; ".join(x for x in t["preconditions"] if "이터레이션" in x) + ")")
        t["status"] = status
        if t["status"].startswith("미실시") and not any("이터레이션" in x for x in t["preconditions"]):
            t["status"] = "미실시 (이터레이션 미정)"
    spec_tcs = []
    for t in TCS:
        d = {k: v for k, v in t.items() if k not in ("exec", "status")}
        d["preconditions"] = [p for p in d["preconditions"] if p]
        d["environment"] = d["environment"] + f" | 실행 상태: {t['status']}"
        if not d["pict_model"]:
            d.pop("pict_model")
        spec_tcs.append(d)
    ep_techs = {"동치 분할", "경계값 분석"}
    covered_ep = {b for t in TCS if ep_techs & set(t["techniques"]) for b in t["basis_ids"]}
    na_reason = {"기능": "입력 범위가 없는 조합·이벤트 규칙: 페어와이즈·상태 전이·WoW 로 다룸",
                 "비기능": "입력 범위가 없는 검사·측정·재생 항목 (성능 측정, 검사, 재생으로 검증)"}
    for b in basis:
        if b["id"] not in covered_ep:
            b["ep_bva_na_reason"] = na_reason[b["type"]]
    spec = {"project": "전자식 차일드락 제어 SW", "basis_document": "docs/requirements/software-requirements.md v0.2, docs/safety/safety-requirements.md v0.2",
            "basis": basis, "test_cases": spec_tcs,
            "pict_models": [{"id": "PW-01", "basis_ids": ["REQ-F-012", "REQ-F-020"], "command": PICT["command"], "model_text": PICT["model_text"],
                             "rows": PICT["rows"], "notes": "이전 확정 출력은 좌 LOCK, 우 RELEASE 로 고정. 기대 출력은 SRS 1.3절 규칙표에서 계산(gen_system_tests.py expected_for)."}],
            "assumptions": [
                {"id": "A-ST-01", "type": "가정", "related_basis": ["REQ-S-003"], "content": "평가 시각과 timestamp 의 비교는 초 단위 부동소수이며, 경계 k=20(나이 0.2 s)은 부동소수 오차 때문에 시험 판정에서 제외한다. 정확한 경계는 단위 시험 UT-input-011/012 가 이진 정확값으로 검증한다.", "impact": "ST-NF-001"},
                {"id": "A-ST-02", "type": "질의", "related_basis": ["REQ-F-023", "REQ-S-002"], "content": "OQ-02(override 와 ASIL B 억제 상충)는 OEM 확인 전이며 override TC 의 기대값은 잠정이다.", "impact": "override 관련 TC"},
                {"id": "A-ST-03", "type": "질의", "related_basis": ["REQ-S-001", "REQ-S-004"], "content": "OQ-01(sensor_fault 와 충돌 동시 발생)은 OEM 확인 전이며 PICT 기대값은 SRS 규칙표(sensor_fault 우선)를 따른다.", "impact": "PW-01 조합 중 sensor_fault=1 이면서 crash=CONFIRMED"}]}
    out_dir = REPO / "docs/test/system"
    (out_dir / "system-test-spec.json").write_text(json.dumps(spec, ensure_ascii=False, indent=1), encoding="utf-8", newline="\n")

    # 실행 시험 (C++)
    cpp = ["/**\n * @file    ecl_system_test.cpp\n * @brief   시스템 테스트: controller 인터페이스(ARCH-I-006)에서 요구사항을 실제 컴포넌트로 검증한다 (이터레이션 1 실행분).\n *\n" + HITL + " */\n",
           "#include <gtest/gtest.h>\n\n#include <chrono>\n\n", '#include "ecl/controller.h"\n#include "../support/ecl_test_support.hpp"\n\nnamespace\n{\n\n']
    for t in TCS:
        if t["exec"] is None:
            continue
        name = t["id"].replace("-", "_")
        cpp.append("/**\n" + f" * @test      {t['id']}\n * @brief     [목적] {t['purpose']}\n * @satisfies {', '.join(t['basis_ids'])}\n"
                   f" * @technique {', '.join(t['techniques'])}\n * @input     {json.dumps(t['inputs'], ensure_ascii=False)}\n"
                   f" * @procedure {' / '.join(t['procedure'])}\n * @expected  {t['expected']}\n *\n" + HITL + " */\n")
        body = "\n".join(("    " + ln) if ln else "" for ln in cpp_for(t))
        cpp.append(f"TEST(EclSystemTest, {name})\n{{\n{body}\n}}\n\n")
    cpp.append("}  // namespace\n")
    target = REPO / "test/system/ecl_system_test.cpp"
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_text("".join(cpp), encoding="utf-8", newline="\n")
    (REPO / "test/system/CMakeLists.txt").write_text("cc_add_test_suite(system\n    SOURCES ecl_system_test.cpp\n    LIBS ecl_core)\n", encoding="utf-8", newline="\n")

    # 케이스 목록 문서
    md = ["# 시스템 테스트 케이스 목록과 실행 상태", "",
          "작성 정보: claude-sonnet-5-5, 2026-11-27, 요청자 DongJoonHan. 사용자 리뷰: 미완료. 리뷰: 이터레이션 1 면제.", "",
          "- 명세 원본: `system-test-spec.json` → `system-test-spec.xlsx` (기능/비기능 시트, 추적성, 커버리지, PICT 모델).",
          "- 실행 시험: `test/system/ecl_system_test.cpp` (이터레이션 1 범위). 나머지는 해당 이터레이션에서 실행한다.",
          "- 기대값은 SRS 에서 직접 도출했다 (구현 출력을 베끼지 않음).", "",
          "| ST ID | 베이시스 | 구분 | 실행 상태 |", "|---|---|---|---|"]
    for t in TCS:
        md.append(f"| {t['id']} | {', '.join(t['basis_ids'])} | {t['category']} | {t['status']} |")
    (out_dir / "system-test-cases.md").write_text("\n".join(md) + "\n", encoding="utf-8", newline="\n")

    # 추적 매트릭스 ST 열 갱신
    cov = {}
    for t in TCS:
        for b in t["basis_ids"]:
            cov.setdefault(b, []).append(t["id"])
    def compress(idlist):
        return ", ".join(idlist) if len(idlist) <= 3 else f"{idlist[0]}..{idlist[-1]} ({len(idlist)}건)"
    (out_dir / "st-coverage.json").write_text(json.dumps({k: v for k, v in cov.items()}, ensure_ascii=False, indent=1), encoding="utf-8", newline="\n")
    print("TC", len(TCS), "basis", len(basis), "uncovered", sorted(ids - set(cov)))


if __name__ == "__main__":
    main()
