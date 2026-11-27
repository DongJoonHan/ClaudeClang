/**
 * @file    command_intake_test.cpp
 * @brief   command_intake 단위 테스트 (MC/DC 100% 대상).
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
#include <gtest/gtest.h>
#include "ecl/command_intake.h"
#include "../support/ecl_test_support.hpp"

namespace
{

/**
 * @test      UT-cmd-001
 * @brief     [목적] out 이 널이면 ECL_ERR_NULL_ARG 를 반환한다.
 * @satisfies DD-M-002
 * @technique MC/DC(조건 out == NULL 의 독립 영향)
 * @input     out=nullptr, cmds=유효, count=1
 * @procedure 1) ecl_command_intake() 를 호출한다.
 * @expected  반환값 == ECL_ERR_NULL_ARG
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(CommandIntakeTest, ReturnsErrNullArgWhenOutIsNull)
{
    const ecl_raw_command_t cmd = ValidCommand();
    EXPECT_EQ(ecl_command_intake(&cmd, 1, 1.0, nullptr), ECL_ERR_NULL_ARG);
}

/**
 * @test      UT-cmd-002
 * @brief     [목적] cmds 가 널인데 count 가 0 초과이면 ECL_ERR_NULL_ARG 이다.
 * @satisfies DD-M-002
 * @technique MC/DC(조건 cmds == NULL, count > 0 의 독립 영향)
 * @input     cmds=nullptr, count=1
 * @procedure 1) 호출한다.
 * @expected  반환값 == ECL_ERR_NULL_ARG
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(CommandIntakeTest, ReturnsErrNullArgWhenCmdsIsNullButCountPositive)
{
    ecl_command_result_t out{};
    EXPECT_EQ(ecl_command_intake(nullptr, 1, 1.0, &out), ECL_ERR_NULL_ARG);
}

/**
 * @test      UT-cmd-003
 * @brief     [목적] 명령이 없으면(cmds 널, count 0) 채택 없음이고 오류가 아니다.
 * @satisfies REQ-F-007
 * @technique MC/DC(조건 count > 0 의 독립 영향) + 동치 클래스(명령 없음)
 * @input     cmds=nullptr, count=0, out=센티널
 * @procedure 1) out.accepted=9 로 둔다. 2) 호출한다.
 * @expected  ECL_OK, accepted == 0, note == NONE, extra_rejected == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(CommandIntakeTest, NoCommandsYieldsEmptyResult)
{
    ecl_command_result_t out{};
    out.accepted = 9;
    EXPECT_EQ(ecl_command_intake(nullptr, 0, 1.0, &out), ECL_OK);
    EXPECT_EQ(out.accepted, 0);
    EXPECT_EQ(out.note, ECL_REASON_NONE);
    EXPECT_EQ(out.extra_rejected, 0U);
}

/**
 * @test      UT-cmd-004
 * @brief     [목적] side 3종, action 2종, source 4종의 모든 조합을 채택하고 요청 시각을 기록한다.
 * @satisfies REQ-F-006, REQ-F-018, REQ-F-019, REQ-F-008, REQ-F-034
 * @technique 동치 클래스 전수(3x2x4)
 * @input     side∈0..2, action∈0..1, source∈0..3, 시각 12.5
 * @procedure 1) 조합마다 호출한다.
 * @expected  accepted == 1, 값 보존, request_time_s == 12.5, note == NONE
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(CommandIntakeTest, AcceptsEveryValidSideActionSourceCombination)
{
    for (int side = 0; side <= 2; ++side)
    {
        for (int action = 0; action <= 1; ++action)
        {
            for (int source = 0; source <= 3; ++source)
            {
                ecl_raw_command_t cmd{};
                cmd.side = Int(side);
                cmd.action = Int(action);
                cmd.source = Int(source);
                ecl_command_result_t out{};
                ASSERT_EQ(ecl_command_intake(&cmd, 1, 12.5, &out), ECL_OK);
                EXPECT_EQ(out.accepted, 1);
                EXPECT_EQ(out.side, static_cast<ecl_side_t>(side));
                EXPECT_EQ(out.action, static_cast<ecl_action_t>(action));
                EXPECT_EQ(out.source, static_cast<ecl_source_t>(source));
                EXPECT_DOUBLE_EQ(out.request_time_s, 12.5);
                EXPECT_EQ(out.note, ECL_REASON_NONE);
            }
        }
    }
}

/**
 * @test      UT-cmd-005
 * @brief     [목적] side 가 범위 밖이거나 누락이면 명령을 거절한다.
 * @satisfies REQ-F-006
 * @technique MC/DC(조건 present, 하한, 상한의 독립 영향) + 경계값 분석
 * @input     side ∈ {-1, 3, 누락}, 나머지 유효
 * @procedure 1) 각 입력으로 호출한다.
 * @expected  accepted == 0, note == CMD_REJECTED_INVALID, extra_rejected == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(CommandIntakeTest, RejectsSideOutOfRangeOrMissing)
{
    const ecl_raw_int_t cases[] = {Int(-1), Int(3), MissingInt()};
    for (const ecl_raw_int_t& bad : cases)
    {
        ecl_raw_command_t cmd = ValidCommand();
        cmd.side = bad;
        ecl_command_result_t out{};
        ASSERT_EQ(ecl_command_intake(&cmd, 1, 1.0, &out), ECL_OK);
        EXPECT_EQ(out.accepted, 0);
        EXPECT_EQ(out.note, ECL_REASON_CMD_REJECTED_INVALID);
        EXPECT_EQ(out.extra_rejected, 0U);
    }
}

/**
 * @test      UT-cmd-006
 * @brief     [목적] action 가 범위 밖이거나 누락이면 명령을 거절한다.
 * @satisfies REQ-F-018
 * @technique MC/DC(조건 present, 하한, 상한의 독립 영향) + 경계값 분석
 * @input     action ∈ {-1, 2, 누락}, 나머지 유효
 * @procedure 1) 각 입력으로 호출한다.
 * @expected  accepted == 0, note == CMD_REJECTED_INVALID, extra_rejected == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(CommandIntakeTest, RejectsActionOutOfRangeOrMissing)
{
    const ecl_raw_int_t cases[] = {Int(-1), Int(2), MissingInt()};
    for (const ecl_raw_int_t& bad : cases)
    {
        ecl_raw_command_t cmd = ValidCommand();
        cmd.action = bad;
        ecl_command_result_t out{};
        ASSERT_EQ(ecl_command_intake(&cmd, 1, 1.0, &out), ECL_OK);
        EXPECT_EQ(out.accepted, 0);
        EXPECT_EQ(out.note, ECL_REASON_CMD_REJECTED_INVALID);
        EXPECT_EQ(out.extra_rejected, 0U);
    }
}

/**
 * @test      UT-cmd-007
 * @brief     [목적] source 가 범위 밖이거나 누락이면 명령을 거절한다.
 * @satisfies REQ-F-019
 * @technique MC/DC(조건 present, 하한, 상한의 독립 영향) + 경계값 분석
 * @input     source ∈ {-1, 4, 누락}, 나머지 유효
 * @procedure 1) 각 입력으로 호출한다.
 * @expected  accepted == 0, note == CMD_REJECTED_INVALID, extra_rejected == 0
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(CommandIntakeTest, RejectsSourceOutOfRangeOrMissing)
{
    const ecl_raw_int_t cases[] = {Int(-1), Int(4), MissingInt()};
    for (const ecl_raw_int_t& bad : cases)
    {
        ecl_raw_command_t cmd = ValidCommand();
        cmd.source = bad;
        ecl_command_result_t out{};
        ASSERT_EQ(ecl_command_intake(&cmd, 1, 1.0, &out), ECL_OK);
        EXPECT_EQ(out.accepted, 0);
        EXPECT_EQ(out.note, ECL_REASON_CMD_REJECTED_INVALID);
        EXPECT_EQ(out.extra_rejected, 0U);
    }
}

/**
 * @test      UT-cmd-008
 * @brief     [목적] 한 주기에 유효한 명령이 2건이면 첫 건만 채택하고 나머지는 CMD_REJECTED_EXTRA 로 기록한다.
 * @satisfies REQ-F-009
 * @technique MC/DC(조건 count > 1, accepted 의 독립 영향)
 * @input     명령 2건(좌 잠금, 우 해제)
 * @procedure 1) 호출한다.
 * @expected  accepted == 1, side == LEFT, note == CMD_REJECTED_EXTRA, extra_rejected == 1
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(CommandIntakeTest, SecondCommandIsRejectedAsExtra)
{
    ecl_raw_command_t cmds[2] = {ValidCommand(), ValidCommand()};
    cmds[1].side = Int(ECL_SIDE_RIGHT);
    cmds[1].action = Int(ECL_ACTION_UNLOCK);
    ecl_command_result_t out{};
    ASSERT_EQ(ecl_command_intake(cmds, 2, 1.0, &out), ECL_OK);
    EXPECT_EQ(out.accepted, 1);
    EXPECT_EQ(out.side, ECL_SIDE_LEFT);
    EXPECT_EQ(out.note, ECL_REASON_CMD_REJECTED_EXTRA);
    EXPECT_EQ(out.extra_rejected, 1U);
}

/**
 * @test      UT-cmd-009
 * @brief     [목적] 첫 명령이 무효이면 추가 명령 수를 세되 note 는 CMD_REJECTED_INVALID 를 유지한다.
 * @satisfies REQ-F-007, REQ-F-009
 * @technique MC/DC(조건 accepted 의 독립 영향)
 * @input     첫 명령 side 누락, 총 3건
 * @procedure 1) 호출한다.
 * @expected  accepted == 0, note == CMD_REJECTED_INVALID, extra_rejected == 2
 *
 * @ai_model        claude-sonnet-5-5
 * @ai_date         2026-11-27
 * @requested_by    DongJoonHan
 * @human_reviewed  no
 * @reviewed_by     -
 * @reviewed_date   -
 */
TEST(CommandIntakeTest, InvalidFirstCommandKeepsInvalidNoteAndCountsExtras)
{
    ecl_raw_command_t cmds[3] = {ValidCommand(), ValidCommand(), ValidCommand()};
    cmds[0].side = MissingInt();
    ecl_command_result_t out{};
    ASSERT_EQ(ecl_command_intake(cmds, 3, 1.0, &out), ECL_OK);
    EXPECT_EQ(out.accepted, 0);
    EXPECT_EQ(out.note, ECL_REASON_CMD_REJECTED_INVALID);
    EXPECT_EQ(out.extra_rejected, 2U);
}

}  // namespace
