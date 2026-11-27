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

#include "smoke/controller.h"

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
struct FixedSensor
{
    int value;              ///< 돌려줄 값
    smoke_status_t status;  ///< 돌려줄 상태
    int calls;              ///< read가 호출된 횟수
};

/**
 * @brief   FixedSensor의 고정된 값과 상태를 돌려준다 (센서 인터페이스 구현).
 * @param   context    FixedSensor 포인터
 * @param   out_value  값을 돌려받는 출력 인자
 * @return  FixedSensor에 지정한 상태 (호출 횟수를 기록한다)
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
smoke_status_t ReadFixed(void* context, int* out_value)
{
    FixedSensor* fixed = static_cast<FixedSensor*>(context);
    ++fixed->calls;
    *out_value = fixed->value;
    return fixed->status;
}

/**
 * @test      IT-controller_clamp-001
 * @brief     [목적] 센서 값이 Controller와 clamp를 거쳐 제한된 명령이 된다.
 * @satisfies ARCH-I-002, REQ-F-002
 * @technique 동치 클래스(범위 아래/안/위) + 데이터 흐름 전 구간
 * @input     센서 값 ∈ {-5, 5, 50}, low=0, high=10
 * @procedure 1) 값을 돌려주는 센서를 만든다. 2) 실제 smoke_controller_step()을 호출한다.
 * @expected  반환값 == SMOKE_OK, 명령 ∈ {0, 5, 10}
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
    smoke_controller_t controller{};
    ASSERT_EQ(smoke_controller_init(&controller, 0, 10), SMOKE_OK);
    const int inputs[] = {-5, 5, 50};
    const int expected[] = {0, 5, 10};
    for (int i = 0; i < 3; ++i)
    {
        FixedSensor fixed{inputs[i], SMOKE_OK, 0};
        const smoke_sensor_t sensor{&ReadFixed, &fixed};
        int out = -99;
        EXPECT_EQ(smoke_controller_step(&controller, &sensor, &out), SMOKE_OK);
        EXPECT_EQ(out, expected[i]);
        EXPECT_EQ(fixed.calls, 1);
    }
}

/**
 * @test      IT-controller_clamp-002
 * @brief     [목적] 설정 오류(low > high)가 clamp에서 Controller로 전파된다.
 * @satisfies ARCH-I-002, REQ-F-002
 * @technique 오류 전파
 * @input     low=10, high=0, 센서 값=5
 * @procedure 1) 잘못된 범위의 Controller를 만든다. 2) step()을 호출한다.
 * @expected  반환값 == SMOKE_ERR_RANGE
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
    smoke_controller_t controller{};
    ASSERT_EQ(smoke_controller_init(&controller, 10, 0), SMOKE_OK);
    FixedSensor fixed{5, SMOKE_OK, 0};
    const smoke_sensor_t sensor{&ReadFixed, &fixed};
    int out = -99;
    EXPECT_EQ(smoke_controller_step(&controller, &sensor, &out), SMOKE_ERR_RANGE);
}

/**
 * @test      IT-controller_clamp-003
 * @brief     [목적] 센서 오류가 Controller 밖으로 그대로 전달된다.
 * @satisfies ARCH-I-001, REQ-F-002
 * @technique 오류 전파
 * @input     센서가 SMOKE_ERR_RANGE 반환
 * @procedure 1) 오류를 돌려주는 센서로 step()을 호출한다.
 * @expected  반환값 == SMOKE_ERR_RANGE
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
    smoke_controller_t controller{};
    ASSERT_EQ(smoke_controller_init(&controller, 0, 10), SMOKE_OK);
    FixedSensor fixed{5, SMOKE_ERR_RANGE, 0};
    const smoke_sensor_t sensor{&ReadFixed, &fixed};
    int out = -99;
    EXPECT_EQ(smoke_controller_step(&controller, &sensor, &out), SMOKE_ERR_RANGE);
}

}  // namespace
