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

#include "smoke/controller.hpp"

namespace
{

using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::_;

/**
 * @brief   ISensor의 GoogleMock 대체물.
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
class MockSensor : public smoke::ISensor
{
public:
    MOCK_METHOD(smoke::Status, read, (int* out_value), (override));
};

/**
 * @test      UT-controller-001
 * @brief     [목적] sensor가 널이면 ErrNullArg를 반환한다.
 * @satisfies REQ-F-002
 * @technique MC/DC(조건 sensor == nullptr 의 독립 영향)
 * @input     sensor=nullptr, out_command=유효
 * @procedure 1) step(nullptr, &out)을 호출한다.
 * @expected  반환값 == Status::ErrNullArg
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
    const smoke::Controller controller(0, 10);
    int out = 0;
    EXPECT_EQ(controller.step(nullptr, &out), smoke::Status::ErrNullArg);
}

/**
 * @test      UT-controller-002
 * @brief     [목적] out_command가 널이면 센서를 읽지 않고 ErrNullArg를 반환한다.
 * @satisfies REQ-F-002
 * @technique MC/DC(조건 out_command == nullptr 의 독립 영향)
 * @input     sensor=유효, out_command=nullptr
 * @procedure 1) read()가 호출되지 않을 것을 기대하고 step(&sensor, nullptr)을 호출한다.
 * @expected  반환값 == Status::ErrNullArg, read() 호출 0회
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
    const smoke::Controller controller(0, 10);
    MockSensor sensor;
    EXPECT_CALL(sensor, read(_)).Times(0);
    EXPECT_EQ(controller.step(&sensor, nullptr), smoke::Status::ErrNullArg);
}

/**
 * @test      UT-controller-003
 * @brief     [목적] 센서 오류는 그대로 전달되고 명령은 바뀌지 않는다.
 * @satisfies REQ-F-002
 * @technique 오류 경로
 * @input     센서가 Status::ErrRange 반환, out=센티널(-99)
 * @procedure 1) read()가 ErrRange를 반환하게 설정한다. 2) step()을 호출한다.
 * @expected  반환값 == Status::ErrRange, out == -99
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
    const smoke::Controller controller(0, 10);
    MockSensor sensor;
    EXPECT_CALL(sensor, read(_)).WillOnce(Return(smoke::Status::ErrRange));
    int out = -99;
    EXPECT_EQ(controller.step(&sensor, &out), smoke::Status::ErrRange);
    EXPECT_EQ(out, -99);
}

/**
 * @test      UT-controller-004
 * @brief     [목적] 센서 값이 범위 밖이면 제한된 명령을, 안이면 그대로 만든다.
 * @satisfies REQ-F-002
 * @technique 동치 클래스(범위 아래/안/위)
 * @input     센서 값 ∈ {-5, 5, 50}, low=0, high=10
 * @procedure 1) 각 값을 돌려주도록 센서를 설정한다. 2) step()을 호출한다.
 * @expected  반환값 == Status::Ok, out ∈ {0, 5, 10}
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
    const smoke::Controller controller(0, 10);
    const int inputs[] = {-5, 5, 50};
    const int expected[] = {0, 5, 10};
    for (int i = 0; i < 3; ++i)
    {
        MockSensor sensor;
        EXPECT_CALL(sensor, read(_))
            .WillOnce(DoAll(SetArgPointee<0>(inputs[i]), Return(smoke::Status::Ok)));
        int out = -99;
        EXPECT_EQ(controller.step(&sensor, &out), smoke::Status::Ok);
        EXPECT_EQ(out, expected[i]);
    }
}

}  // namespace
