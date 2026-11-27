/**
 * @file    controller.hpp
 * @brief   센서 값을 읽어 범위로 제한하는 제어기의 공개 인터페이스 (툴체인 검증용 참조 코드).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#ifndef SMOKE_CONTROLLER_HPP
#define SMOKE_CONTROLLER_HPP

#include "smoke/clamp.hpp"

namespace smoke
{

/**
 * @brief   센서 인터페이스. 제어기는 이 인터페이스로만 센서와 통신한다.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
class ISensor
{
public:
    /**
     * @brief   센서 값을 읽는다.
     * @param   out_value  읽은 값을 돌려받는 출력 인자
     * @return  Status::Ok 또는 오류 상태
     * @satisfies ARCH-I-001
     *
     * @ai_model        claude-sonnet-5-5
     * @ai_date         2026-11-27
     * @requested_by    DongJoonHan
     * @human_reviewed  no
     * @reviewed_by     -
     * @reviewed_date   -
     */
    virtual Status read(int* out_value) = 0;

protected:
    // 인터페이스는 기반 클래스 포인터로 삭제하지 않는다 (C++ Core Guidelines C.35).
    // protected non-virtual 소멸자는 실행 가능한 함수가 아니므로 함수 커버리지의 "도달 불가 함수"가 되지 않는다.
    ~ISensor() = default;
};

/**
 * @brief   센서 값을 읽어 [low, high] 범위로 제한한 명령을 만드는 제어기.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
class Controller
{
public:
    /**
     * @brief   제어기를 만든다.
     * @param   low   명령 하한
     * @param   high  명령 상한
     *
     * @ai_model        claude-sonnet-5-5
     * @ai_date         2026-11-27
     * @requested_by    DongJoonHan
     * @human_reviewed  no
     * @reviewed_by     -
     * @reviewed_date   -
     */
    Controller(int low, int high);

    /**
     * @brief   센서를 한 번 읽어 범위로 제한한 명령을 만든다.
     * @param   sensor       센서 인터페이스
     * @param   out_command  명령을 돌려받는 출력 인자
     * @return  Status::Ok, 인자가 널이면 Status::ErrNullArg, 센서 오류는 그대로 전달
     * @satisfies REQ-F-002, ARCH-I-002
     *
     * @ai_model        claude-sonnet-5-5
     * @ai_date         2026-11-27
     * @requested_by    DongJoonHan
     * @human_reviewed  no
     * @reviewed_by     -
     * @reviewed_date   -
     */
    [[nodiscard]] Status step(ISensor* sensor, int* out_command) const;

private:
    int low_;   ///< 명령 하한
    int high_;  ///< 명령 상한
};

}  // namespace smoke

#endif  // SMOKE_CONTROLLER_HPP
