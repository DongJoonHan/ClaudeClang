/**
 * @file    controller_test.cpp
 * @brief   Controller 단위 테스트 (센서는 GoogleMock으로 대체, MC/DC 100% 대상).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "smoke/controller.h"

namespace
{

using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::_;

/**
 * @brief   센서 인터페이스(함수 포인터)의 GoogleMock 대체물.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
class MockSensor
{
public:
    MOCK_METHOD(smoke_status_t, Read, (int* out_value));

    /**
     * @brief   C 함수 포인터 형식의 어댑터. 문맥을 MockSensor로 되돌려 Read()에 위임한다.
     * @param   context    MockSensor 포인터
     * @param   out_value  읽은 값을 돌려받는 출력 인자
     * @return  Read()의 반환 상태
     *
     * @ai_model        claude-sonnet-5-5
     * @ai_date         2026-11-27
     * @requested_by    DongJoonHan
     * @human_reviewed  no
     * @reviewed_by     -
     * @reviewed_date   -
     */
    static smoke_status_t Adapter(void* context, int* out_value)
    {
        return static_cast<MockSensor*>(context)->Read(out_value);
    }

    /**
     * @brief   제어기에 주입할 센서 인터페이스를 만든다.
     * @return  Adapter와 이 객체를 묶은 smoke_sensor_t
     *
     * @ai_model        claude-sonnet-5-5
     * @ai_date         2026-11-27
     * @requested_by    DongJoonHan
     * @human_reviewed  no
     * @reviewed_by     -
     * @reviewed_date   -
     */
    smoke_sensor_t Interface()
    {
        return smoke_sensor_t{&MockSensor::Adapter, this};
    }
};

/**
 * @test      UT-controller-001
 * @brief     [목적] controller가 널이면 SMOKE_ERR_NULL_ARG를 반환한다.
 * @satisfies REQ-F-002
 * @technique MC/DC(조건 controller == NULL 의 독립 영향)
 * @input     controller=nullptr, sensor=유효, out_command=유효
 * @procedure 1) smoke_controller_step(nullptr, ...)을 호출한다.
 * @expected  반환값 == SMOKE_ERR_NULL_ARG, Read() 호출 0회
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, ReturnsErrNullArgWhenControllerIsNull)
{
    MockSensor mock;
    const smoke_sensor_t sensor = mock.Interface();
    int out = 0;
    EXPECT_CALL(mock, Read(_)).Times(0);
    EXPECT_EQ(smoke_controller_step(nullptr, &sensor, &out), SMOKE_ERR_NULL_ARG);
}

/**
 * @test      UT-controller-002
 * @brief     [목적] sensor가 널이면 SMOKE_ERR_NULL_ARG를 반환한다.
 * @satisfies REQ-F-002
 * @technique MC/DC(조건 sensor == NULL 의 독립 영향)
 * @input     controller=유효, sensor=nullptr, out_command=유효
 * @procedure 1) smoke_controller_step(&controller, nullptr, &out)을 호출한다.
 * @expected  반환값 == SMOKE_ERR_NULL_ARG
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, ReturnsErrNullArgWhenSensorIsNull)
{
    smoke_controller_t controller{};
    ASSERT_EQ(smoke_controller_init(&controller, 0, 10), SMOKE_OK);
    int out = 0;
    EXPECT_EQ(smoke_controller_step(&controller, nullptr, &out), SMOKE_ERR_NULL_ARG);
}

/**
 * @test      UT-controller-003
 * @brief     [목적] 센서의 read 함수가 널이면 SMOKE_ERR_NULL_ARG를 반환한다.
 * @satisfies REQ-F-002
 * @technique MC/DC(조건 sensor->read == NULL 의 독립 영향)
 * @input     controller=유효, sensor.read=nullptr, out_command=유효
 * @procedure 1) read가 널인 센서로 smoke_controller_step()을 호출한다.
 * @expected  반환값 == SMOKE_ERR_NULL_ARG
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, ReturnsErrNullArgWhenSensorReadIsNull)
{
    smoke_controller_t controller{};
    ASSERT_EQ(smoke_controller_init(&controller, 0, 10), SMOKE_OK);
    const smoke_sensor_t sensor{nullptr, nullptr};
    int out = 0;
    EXPECT_EQ(smoke_controller_step(&controller, &sensor, &out), SMOKE_ERR_NULL_ARG);
}

/**
 * @test      UT-controller-004
 * @brief     [목적] out_command가 널이면 센서를 읽지 않고 SMOKE_ERR_NULL_ARG를 반환한다.
 * @satisfies REQ-F-002
 * @technique MC/DC(조건 out_command == NULL 의 독립 영향)
 * @input     controller=유효, sensor=유효, out_command=nullptr
 * @procedure 1) Read()가 호출되지 않을 것을 기대하고 smoke_controller_step(..., nullptr)을 호출한다.
 * @expected  반환값 == SMOKE_ERR_NULL_ARG, Read() 호출 0회
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, ReturnsErrNullArgWhenOutCommandIsNull)
{
    smoke_controller_t controller{};
    ASSERT_EQ(smoke_controller_init(&controller, 0, 10), SMOKE_OK);
    MockSensor mock;
    const smoke_sensor_t sensor = mock.Interface();
    EXPECT_CALL(mock, Read(_)).Times(0);
    EXPECT_EQ(smoke_controller_step(&controller, &sensor, nullptr), SMOKE_ERR_NULL_ARG);
}

/**
 * @test      UT-controller-005
 * @brief     [목적] 센서 오류는 그대로 전달되고 명령은 바뀌지 않는다.
 * @satisfies REQ-F-002
 * @technique 오류 경로
 * @input     센서가 SMOKE_ERR_RANGE 반환, out=센티널(-99)
 * @procedure 1) Read()가 SMOKE_ERR_RANGE를 반환하게 설정한다. 2) smoke_controller_step()을 호출한다.
 * @expected  반환값 == SMOKE_ERR_RANGE, out == -99
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, PropagatesSensorError)
{
    smoke_controller_t controller{};
    ASSERT_EQ(smoke_controller_init(&controller, 0, 10), SMOKE_OK);
    MockSensor mock;
    const smoke_sensor_t sensor = mock.Interface();
    EXPECT_CALL(mock, Read(_)).WillOnce(Return(SMOKE_ERR_RANGE));
    int out = -99;
    EXPECT_EQ(smoke_controller_step(&controller, &sensor, &out), SMOKE_ERR_RANGE);
    EXPECT_EQ(out, -99);
}

/**
 * @test      UT-controller-006
 * @brief     [목적] 센서 값이 범위 밖이면 제한된 명령을, 안이면 그대로 만든다.
 * @satisfies REQ-F-002
 * @technique 동치 클래스(범위 아래/안/위)
 * @input     센서 값 ∈ {-5, 5, 50}, low=0, high=10
 * @procedure 1) 각 값을 돌려주도록 센서를 설정한다. 2) smoke_controller_step()을 호출한다.
 * @expected  반환값 == SMOKE_OK, out ∈ {0, 5, 10}
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, ProducesClampedCommand)
{
    smoke_controller_t controller{};
    ASSERT_EQ(smoke_controller_init(&controller, 0, 10), SMOKE_OK);
    const int inputs[] = {-5, 5, 50};
    const int expected[] = {0, 5, 10};
    for (int i = 0; i < 3; ++i)
    {
        MockSensor mock;
        const smoke_sensor_t sensor = mock.Interface();
        EXPECT_CALL(mock, Read(_))
            .WillOnce(DoAll(SetArgPointee<0>(inputs[i]), Return(SMOKE_OK)));
        int out = -99;
        EXPECT_EQ(smoke_controller_step(&controller, &sensor, &out), SMOKE_OK);
        EXPECT_EQ(out, expected[i]);
    }
}

/**
 * @test      UT-controller-007
 * @brief     [목적] controller가 널이면 초기화가 SMOKE_ERR_NULL_ARG를 반환한다.
 * @satisfies REQ-F-002
 * @technique MC/DC(조건 controller == NULL 의 독립 영향)
 * @input     controller=nullptr
 * @procedure 1) smoke_controller_init(nullptr, 0, 10)을 호출한다.
 * @expected  반환값 == SMOKE_ERR_NULL_ARG
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, InitReturnsErrNullArgWhenControllerIsNull)
{
    EXPECT_EQ(smoke_controller_init(nullptr, 0, 10), SMOKE_ERR_NULL_ARG);
}

/**
 * @test      UT-controller-008
 * @brief     [목적] 초기화는 범위를 제어기에 저장한다.
 * @satisfies REQ-F-002
 * @technique 동치 클래스(유효 입력)
 * @input     controller=유효, low=-3, high=7
 * @procedure 1) smoke_controller_init()을 호출한다.
 * @expected  반환값 == SMOKE_OK, low == -3, high == 7
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(ControllerTest, InitStoresRange)
{
    smoke_controller_t controller{};
    EXPECT_EQ(smoke_controller_init(&controller, -3, 7), SMOKE_OK);
    EXPECT_EQ(controller.low, -3);
    EXPECT_EQ(controller.high, 7);
}

}  // namespace
