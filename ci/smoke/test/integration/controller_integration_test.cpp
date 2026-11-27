/**
 * @file    controller_integration_test.cpp
 * @brief   Controller와 clamp를 실제 인터페이스로 연결하는 통합 테스트 (함수 커버리지 100% 대상).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include <gtest/gtest.h>

#include "smoke/controller.hpp"

namespace
{

/**
 * @brief   시험 범위 밖 경계(하드웨어 센서)를 대신하는 고정값 센서.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
class FixedSensor : public smoke::ISensor
{
public:
    /**
     * @brief   센서를 만든다.
     * @param   value   read()가 돌려줄 값
     * @param   status  read()가 돌려줄 상태
     *
     * @ai_model        claude-sonnet-5-5
     * @ai_date         2026-11-27
     * @requested_by    DongJoonHan
     * @human_reviewed  no
     * @reviewed_by     -
     * @reviewed_date   -
     */
    FixedSensor(int value, smoke::Status status) : value_(value), status_(status)
    {
    }

    /**
     * @brief   고정된 값과 상태를 돌려준다.
     * @param   out_value  값을 돌려받는 출력 인자
     * @return  생성 시 지정한 상태
     *
     * @ai_model        claude-sonnet-5-5
     * @ai_date         2026-11-27
     * @requested_by    DongJoonHan
     * @human_reviewed  no
     * @reviewed_by     -
     * @reviewed_date   -
     */
    smoke::Status read(int* out_value) override
    {
        *out_value = value_;
        return status_;
    }

private:
    int value_;             ///< 돌려줄 값
    smoke::Status status_;  ///< 돌려줄 상태
};

/**
 * @test      IT-controller_clamp-001
 * @brief     [목적] 센서 값이 Controller와 clamp를 거쳐 제한된 명령이 된다.
 * @satisfies ARCH-I-002, REQ-F-002
 * @technique 동치 클래스(범위 아래/안/위) + 데이터 흐름 전 구간
 * @input     센서 값 ∈ {-5, 5, 50}, low=0, high=10
 * @procedure 1) 값을 돌려주는 센서를 만든다. 2) 실제 Controller::step()을 호출한다.
 * @expected  반환값 == Status::Ok, 명령 ∈ {0, 5, 10}
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerIntegrationTest, FlowsSensorValueThroughClampToCommand)
{
    const smoke::Controller controller(0, 10);
    const int inputs[] = {-5, 5, 50};
    const int expected[] = {0, 5, 10};
    for (int i = 0; i < 3; ++i)
    {
        FixedSensor sensor(inputs[i], smoke::Status::Ok);
        int out = -99;
        EXPECT_EQ(controller.step(&sensor, &out), smoke::Status::Ok);
        EXPECT_EQ(out, expected[i]);
    }
}

/**
 * @test      IT-controller_clamp-002
 * @brief     [목적] 설정 오류(low > high)가 clamp에서 Controller로 전파된다.
 * @satisfies ARCH-I-002, REQ-F-002
 * @technique 오류 전파
 * @input     low=10, high=0, 센서 값=5
 * @procedure 1) 잘못된 범위의 Controller를 만든다. 2) step()을 호출한다.
 * @expected  반환값 == Status::ErrRange
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerIntegrationTest, PropagatesClampRangeError)
{
    const smoke::Controller controller(10, 0);
    FixedSensor sensor(5, smoke::Status::Ok);
    int out = -99;
    EXPECT_EQ(controller.step(&sensor, &out), smoke::Status::ErrRange);
}

/**
 * @test      IT-controller_clamp-003
 * @brief     [목적] 센서 오류가 Controller 밖으로 그대로 전달된다.
 * @satisfies ARCH-I-001, REQ-F-002
 * @technique 오류 전파
 * @input     센서가 Status::ErrRange 반환
 * @procedure 1) 오류를 돌려주는 센서로 step()을 호출한다.
 * @expected  반환값 == Status::ErrRange
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerIntegrationTest, PropagatesSensorError)
{
    const smoke::Controller controller(0, 10);
    FixedSensor sensor(5, smoke::Status::ErrRange);
    int out = -99;
    EXPECT_EQ(controller.step(&sensor, &out), smoke::Status::ErrRange);
}

}  // namespace
