---
name: unit-test
description: A-SPICE(SWE.4) 기준으로 단위 테스트를 설계·작성·실행할 때 사용. MC/DC 커버리지 100%와 테스트 성공률 100%를 달성할 때까지 의미 있는 테스트 케이스를 반복 추가하고, 경계값 분석·동치 클래스·PICT 조합을 활용하며, LLVM 도구(스킬 내 스크립트)로 MC/DC를 측정해 케이스를 보강한다. GoogleTest/GMock 사용. "단위 테스트", "커버리지", "MC/DC", "SWE.4" 요청에 사용.
---

# 단위 테스트 스킬 (A-SPICE SWE.4)

공통 위치와 ID 규칙은 프로젝트 루트 `CLAUDE.md`를 따른다. 테스트의 기준은 **상세 설계**(와 그 인터페이스 정의)다. 코드가 아니라 설계에서 기대값을 도출한다.

프레임워크는 **Google Test / Google Mock**, 제품 코드는 C(C17), 시험 코드는 C++20(Clang 18+)이다. 제품 헤더의 `extern "C"` 선언으로 C 함수를 호출한다. 단위 테스트는 `test/unit/`에 둔다. 구현 단계의 TDD 테스트(`test/tdd/`)와는 **분리**되며, 이 스킬의 커버리지에 TDD 테스트를 합산하지 않는다.

## 1. 필수 규칙

1. **MC/DC 커버리지 100%, 테스트 성공률 100%를 달성할 때까지 테스트 케이스를 반복해서 추가한다.**
2. 필요하면 Stub/Mock을 쓸 수 있으나 **과도하게 의존하지 않는다.**
3. **커버리지 달성만을 위한 의미 없는 테스트 케이스를 추가하지 않는다.**
4. **모든 테스트 함수에 Doxygen 스타일 주석으로 테스트 목적, 입력값, 절차, 예상 출력값을 쓴다.**
5. **경계값 분석, 동치 클래스, PICT 조합을 적극 활용한다.**
6. **LLVM 도구로 MC/DC를 측정하고 미충족 조건에 대한 케이스를 추가한다.** LLVM 도구/스크립트는 이 스킬 안(`scripts/`)에 둔다.

### 합격 기준 (`CLAUDE.md` 2.1절)
- **MC/DC 커버리지 100%**
- **테스트 성공률 100%** (실패·건너뜀(`DISABLED_`, `GTEST_SKIP`) 테스트가 없어야 한다)
- 예외는 없다. 도달할 수 없어 달성이 불가능한 항목은 예외로 두지 않고 코드나 설계를 고친다 (6절).

## 2. 케이스 도출 방법

모든 케이스는 "어떤 요구/설계 항목을 어떤 기법으로 검증하는가"가 설명되어야 한다.

- **동치 클래스 분할**: 입력을 유효/무효 클래스로 나누고 클래스별 대표값을 1개 이상 시험한다.
- **경계값 분석**: 각 경계에 대해 직전, 경계, 직후 값을 시험한다. 정수 범위, 버퍼 크기, NULL, 0, 빈 입력, 최대/최소를 포함한다.
- **PICT 조합**: 입력 파라미터가 여러 개일 때 PICT로 pairwise(또는 더 높은 차수) 조합을 생성한다. 아래 3절 참고.
- **오류 경로**: 설계에 정의된 모든 오류 반환값에 대응하는 케이스를 둔다.
- **상태 전이**: 상태가 있는 모듈은 유효·무효 전이를 모두 시험한다.
- 기대값은 설계의 사후조건/반환값 정의에서 가져온다. 현재 코드가 내는 값을 베껴서 기대값으로 삼지 않는다.

## 3. PICT 사용

```
pict model.txt            # pairwise 조합 생성
pict model.txt /o:3       # 3-way 조합
```

모델 예:

```
mode:   INIT, RUN, STOP
level:  -1, 0, 1, 255, 256
enable: true, false
IF [mode] = "INIT" THEN [enable] = "false";
```

- 모델 파일은 테스트 옆(`test/unit/<module>/pict/`)에 두고, 생성된 조합표를 케이스에 연결한다.
- 불가능한 조합은 제약(`IF ... THEN`)으로 제외하고 이유를 주석으로 남긴다.
- PICT가 없으면 설치가 필요함을 보고한다 (https://github.com/microsoft/pict). 임의로 조합을 대충 만들어 "PICT 사용"이라고 기록하지 않는다.

## 4. 테스트 함수 주석 형식 (필수)

```cpp
/**
 * @test      UT-sensor_read-003
 * @brief     [목적] out_value가 NULL일 때 오류 상태를 반환하고 출력을 건드리지 않는다.
 * @satisfies DD-M-012, REQ-F-045
 * @technique 동치 클래스(무효 입력)
 * @input     sensor = 유효한 핸들, out_value = NULL
 * @procedure 1) 유효한 sensor 핸들을 준비한다.
 *            2) out_value = NULL로 sensor_read()를 호출한다.
 * @expected  반환값 == PROJ_ERR_NULL_ARG, 하드웨어 읽기 함수는 호출되지 않음
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(SensorReadTest, ReturnsErrNullArgWhenOutValueIsNull)
```

- 위 다섯 항목(목적, 입력값, 절차, 예상 출력값 + 추적 ID)은 하나도 빠지면 안 된다.
- 시험 코드도 소스 코드이므로 Human-in-the-Loop 태그(`@ai_model`, `@ai_date`, `@requested_by`, `@human_reviewed`, `@reviewed_by`, `@reviewed_date`)를 포함한다. 규칙은 `CLAUDE.md` 2.4절을 따른다 (AI가 `@human_reviewed yes`를 스스로 쓰지 않는다).
- 테스트 ID(`UT-`)와 설계/요구사항 ID를 연결해 추적 매트릭스에 반영한다.
- 테스트 함수도 이름이 의도를 드러내야 한다.

## 5. Stub/Mock 지침 (Google Mock)

대체는 인터페이스(함수 포인터 + `void* context` 구조체의 어댑터, 또는 링크 시점 대체)를 통해 하고, Google Mock(`MOCK_METHOD`)을 사용한다. 어댑터 예: `ci/smoke/test/unit/controller_test.cpp`의 `MockSensor`.

- 대상 모듈이 의존하는 **다른 모듈의 인터페이스**만 대체한다. 시험 대상의 내부 함수를 Mock하지 않는다.
- 하드웨어·OS·시간·난수처럼 단위 시험이 통제할 수 없는 외부 의존에 우선 사용한다.
- Mock으로 호출 횟수·순서를 검증하는 것은 설계에 규정된 상호작용일 때만 한다. 구현 세부를 고정시키는 Mock 검증은 피한다.
- 한 테스트에서 Mock이 너무 많이 필요하면 설계(결합도)를 의심하고 설계 수정을 제안한다.

## 6. 커버리지 반복 절차

```
repeat:
  1. 설계 기반 케이스 도출 (2절) → 작성 → 실행
  2. 커버리지 측정 (7절 스크립트)
  3. 미커버 항목 각각에 대해 판단:
       a) 설계상 도달 가능, 아직 시험 안 한 동작 → 의미 있는 케이스 추가
       b) 도달 불가 코드(죽은 코드, 방어 코드), 또는 조건이 너무 많아 MC/DC로 측정되지 않는 결정
          → 코드를 제거하거나 결정을 분해하도록 설계/코드를 수정한다.
          커버리지 예외는 없다 (CLAUDE.md 2절). 억지 케이스 금지
       c) 설계에 없는 동작 → 설계 결함. 설계부터 수정
until MC/DC 100% 그리고 테스트 성공률 100%
```

- 100% 미달 상태로 "완료"라고 보고하지 않는다. 남은 항목과 사유를 그대로 보고한다.
- 테스트 중 발견한 결함은 테스트를 고치지 말고 결함으로 보고한다.

## 7. 커버리지 측정과 MC/DC (LLVM)

스크립트: `scripts/coverage.sh` (이 스킬 안에 있다).

```
bash .claude/skills/unit-test/scripts/coverage.sh --out build/cov [--mcdc] -- <clang 컴파일 인자...>
```

- LLVM 소스 기반 커버리지(`-fprofile-instr-generate -fcoverage-mapping`)로 라인·영역·분기 커버리지를 측정하고, `llvm-cov`로 리포트와 미커버 위치를 출력한다.
- 단위 테스트는 **항상 `--mcdc`로 실행**한다. `-fcoverage-mcdc` 로 MC/DC를 측정한다 (LLVM/Clang 18 이상 필요). 조건 수가 많은 결정(기본 6개 초과)은 LLVM 제약으로 측정되지 않을 수 있으니 리포트의 경고를 확인하고, 해당 결정은 분해한다.
- 출력 디렉터리는 단위 테스트 `build/cov-unit`을 쓴다. TDD 테스트는 `build/cov-tdd`로 따로 측정하며, 두 결과를 합치지 않는다.
- 미커버 MC/DC 조건쌍은 `llvm-cov show --show-mcdc` 출력에서 어느 조건이 독립적 영향을 보이지 못했는지 보고, **그 조건만 뒤집는 입력쌍**을 설계에서 도출해 케이스를 추가한다.
- 스크립트가 지원하지 않는 사항이 생기면 이 스킬의 `scripts/`에 추가·수정한다 (스킬 밖에 흩어진 일회성 명령을 쓰지 않는다).
- 도구 버전(`clang --version`)을 시험 결과 기록에 남긴다.

## 8. 산출물

- `test/unit/` 아래 GoogleTest 시험 코드 (모듈별 하위 디렉터리, PICT 모델 포함). TDD 테스트(`test/tdd/`)와 섞지 않는다.
- `docs/test/unit-test-report.md`: 케이스 목록(ID, 기법, 추적 ID), 실행 결과, 커버리지 결과, 미달/예외 사유, 도구 버전
- `docs/traceability/trace-matrix.md` 갱신 (설계/요구사항 ↔ UT-)
- `docs/review/` 리뷰 기록 (`review` 스킬의 단위 테스트 체크리스트)
