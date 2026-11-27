---
name: implementation
description: A-SPICE(SWE.3 구현) 기준으로 상세 설계를 코드로 구현할 때 사용. 모든 함수는 반드시 값을 반환하고, return은 함수 끝에서 한 번만 사용하며, 반환할 값이 없으면 void 대신 정상 실행 여부를 나타내는 상태값을 반환한다. "구현", "코딩", "함수 작성" 요청에 사용.
---

# 구현 스킬

공통 위치와 ID 규칙은 프로젝트 루트 `CLAUDE.md`를 따른다. 입력은 확정된 상세 설계다. 코드는 설계를 따르고, 설계에 없는 동작을 임의로 추가하지 않는다. 설계가 틀렸거나 부족하면 코드를 바꾸기 전에 설계를 수정하고 리뷰한다.

대상 언어는 **C++ (Clang 18 이상)**다 (`CLAUDE.md` 4절).

구현은 `test-driven-development` 스킬과 함께 **TDD 방식**으로 진행한다. TDD로 만든 테스트는 `test/tdd/`에 두고 단위 테스트(`test/unit/`)와 분리하며, 결과(`docs/test/tdd-report.md`)와 커버리지(`build/cov-tdd`)도 별도로 낸다 (`CLAUDE.md` 1.4절).

## 1. 함수 규칙 (필수)

### 규칙 A. 모든 함수는 반드시 반환값을 가진다
- `void` 반환 함수를 만들지 않는다.

### 규칙 B. 반환할 값이 없으면 상태값을 반환한다
- 정상 실행 여부를 판단할 수 있는 값(상태 코드)을 반환한다.
- 프로젝트 공통 상태 타입을 하나 정의해 일관되게 쓴다 (예: `enum class Status { Ok = 0, ErrNullArg, ErrRange, ... };`). 오류 값의 의미는 상세 설계의 인터페이스 정의와 일치해야 한다.
- 호출하는 쪽은 반환 상태를 반드시 확인한다. C++에서는 `[[nodiscard]]`, GCC/Clang에서는 `__attribute__((warn_unused_result))`로 무시를 컴파일 경고로 만든다.
- 값을 반환하는 함수에서 오류를 표현해야 하면, 반환값은 상태로 하고 결과는 출력 인자(포인터/참조)로 돌려주거나, 값 자체가 오류 표현을 가지는 타입을 쓴다. 설계에서 정한 방식에 따른다.

### 규칙 C. return은 함수의 마지막에 정확히 한 번만 쓴다
- 함수 중간·분기 안에서 `return`을 쓰지 않는다 (early return 금지).
- 결과 변수를 선언하고 경로마다 값을 설정한 뒤 끝에서 한 번 반환한다.
- 오류 선처리는 조기 반환이 아니라 중첩을 얕게 유지하는 구조(가드 조건 + 상태 변수, 작은 함수 분리)로 처리한다.

패턴:

```cpp
[[nodiscard]] Status sensor_read(const Sensor *sensor, std::int32_t *out_value)
{
    Status result = Status::Ok;

    if ((sensor == nullptr) || (out_value == nullptr))
    {
        result = Status::ErrNullArg;
    }
    else
    {
        result = sensor_hw_read(sensor->channel, out_value);
    }

    return result;
}
```

- 반복문 중간 탈출이 필요하면 루프 조건에 상태 확인을 넣거나(`while ((result == Status::Ok) && (i < n))`) 함수를 분리한다.
- `main`도 같은 규칙으로 끝에서 한 번 반환한다.
- 예외: 언어 기능상 불가피한 경우(람다, 템플릿 메타 함수, 생성자·소멸자처럼 반환형이 없는 함수 등)는 설계 문서에 예외와 사유를 적는다. 생성자·소멸자는 반환값을 가질 수 없으므로 자원 획득을 `init()` 상태 반환 함수로 옮기는 것을 권장한다.

## 2. 일반 구현 규칙

- 상세 설계의 이름, 인터페이스, 반환값 정의를 그대로 사용한다.
- 모듈의 공개 인터페이스는 헤더에만 선언하고, 내부 함수·변수는 `static`(C) 또는 익명 namespace(C++)로 숨긴다. 다른 모듈은 인터페이스(공개 헤더)로만 접근한다.
- 입력 인자 검증은 설계의 사전조건에 따라 수행하고, 위반 시 정의된 오류 상태를 반환한다.
- 함수는 한 가지 일, 작은 크기, 명확한 이름, 매직 넘버 금지 (상세 설계 스킬 3절과 동일).
- 주석은 Doxygen 형식이다. 파일 헤더와 모든 함수·클래스 주석에 `@brief`, `@param`, `@return`, 요구사항/설계 ID와 함께 **Human-in-the-Loop 태그**를 반드시 쓴다 (`CLAUDE.md` 2.4절):
  `@ai_model`(작성 모델), `@ai_date`(작성 일시), `@requested_by`(요청자), `@human_reviewed`(`yes`/`no`), `@reviewed_by`, `@reviewed_date`.
  - 새로 쓰는 코드는 `@human_reviewed no`, `@reviewed_by -`, `@reviewed_date -`로 시작한다.
  - **`yes`는 사용자가 리뷰했다고 알려 준 경우에만, 사용자가 알려 준 이름·일자로 쓴다. 스스로 쓰지 않는다.**
  - 리뷰 완료된 코드를 수정하면 `no`로 되돌리고 `@ai_model`, `@ai_date`도 갱신한다.
- 코드에서 요구사항·설계 ID를 추적할 수 있도록 함수 주석에 `@satisfies DD-M-nnn`, `REQ-F-nnn` 형태로 남긴다.
- 컴파일러 경고를 오류로 취급한다 (`-Wall -Wextra -Werror` 등).
- **Cppcheck 지적 0건.** 예외 처리는 없다 — `--suppress`, 소스 내 `cppcheck-suppress`, 억제 파일을 쓰지 않고 코드를 고친다.
- **Lizard: 순환복잡도 10 이하, 함수 80라인 이하.** 구현이 끝날 때 Hook이 검사하고 CI에서도 검사한다. 위반하면 함수를 분리한다 (단일 책임, 단일 return 규칙과 함께 작은 함수로 나누는 것이 자연스럽다).
- 규칙 A·B·C(반환값, 단일 return)는 자동 점검 도구가 없으므로 `Grep` 등으로 점검하고, 리뷰 체크리스트(`review` 스킬의 코드 체크리스트)로 보완한다.

## 3. 절차

1. 상세 설계의 대상 모듈·함수와 반환값 정의 확인 (불명확하면 설계부터 확정)
2. 헤더(인터페이스) 작성 → TDD 반복(실패하는 테스트 작성 → 구현 → 리팩터링) (규칙 A·B·C). 테스트는 `test/tdd/`
3. 컴파일(경고 0) + Cppcheck 0건 + Lizard 기준 충족
4. 추적 정보 갱신 (코드 ↔ 설계 ↔ 요구사항)
5. `unit-test` 스킬로 단위 테스트 작성·실행
6. `review` 스킬로 코드 체크리스트 리뷰

## 4. 산출물

- 소스 코드 (`src/`, 공개 헤더는 `include/`)
- TDD 테스트 (`test/tdd/`)와 별도 결과·커버리지 (`docs/test/tdd-report.md`, `build/cov-tdd`)
- `docs/traceability/trace-matrix.md` 갱신
- `docs/review/` 리뷰 기록
