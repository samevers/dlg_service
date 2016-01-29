#ifndef UNIT_TEST_HPP
#define UNIT_TEST_HPP

#include <stdio.h>
#include <vector>
#include <exception>
#include "text_color_switcher.hpp"
#include "check.h"

class UnitTestError : public std::exception
{
public:
    explicit UnitTestError(const char* message):
        m_message(message)
    {
    }
    virtual const char* what() const throw()
    {
        return m_message;
    }
private:
    const char* m_message;
};

#define UNIT_TEST_SILENCER(sliencer)   DECLARE_CHECK_SILENCER(sliencer)

/**
 * @brief UNIT_TEST_DEFAINE_EXTERN_TYPE   用户自定义UNIT_TEST输出类型
 * struct My
 * {
 *      My(int x,  int y):m_x(x),m_y(y){}
 *
 *      bool  operator == (My& other){
 *          return (m_x==other.m_x)&&(m_y == other.m_y);
 *      }
 *
 *      int     m_x;
 *      int     m_y;
 *  };
 *
 *  UNIT_TEST_DEFAINE_EXTERN_TYPE(My, m, out)
 *  {
 *      return ( out << " X [" << m.m_x << "] Y[ " << m.m_Y << "] ");
 *  }
 *
 *
 *   UNIT_TEST_CASE( test_my_type )
 *   {
 *      My   m1(10, 20);
 *      My   m2(5, 9);
 *      UNIT_TEST_CHECK_EQ(m1, m2) << "   <!-- JUST FOR TEST MY TYPE -->";
 *   }
 * @return
 */

#define UNIT_TEST_DEFAINE_EXTERN_TYPE(TYPE, val, out)  DEFAINE_EXTERN_CHECK_TYPE(TYPE, val, out)
#define UNIT_TEST_REPORT_FAIL(Condition, Expression) \
        do{\
          if (!(Condition)) \
          { \
            UnitTestCase::ReportFail(__FILE__, __LINE__, #Expression); \
          } \
        }while(0)

/// non fatal check
#define UNIT_TEST_CHECK(Condition)\
        UNIT_TEST_REPORT_FAIL(Condition, #Condition);\
        LEVEL_CHECK(INFO, Condition, Condition)\


#define UNIT_TEST_CHECK_OP(A, B, OP)    \
        UNIT_TEST_REPORT_FAIL((A)OP(B), A OP B);\
        LEVEL_CHECK_OP(INFO, A, B, OP)\

#define UNIT_TEST_CHECK_EQ(A, B) UNIT_TEST_CHECK_OP(A, B, ==)
#define UNIT_TEST_CHECK_NE(A, B) UNIT_TEST_CHECK_OP(A, B, !=)
#define UNIT_TEST_CHECK_LT(A, B) UNIT_TEST_CHECK_OP(A, B, < )
#define UNIT_TEST_CHECK_LE(A, B) UNIT_TEST_CHECK_OP(A, B, <=)
#define UNIT_TEST_CHECK_GT(A, B) UNIT_TEST_CHECK_OP(A, B, > )
#define UNIT_TEST_CHECK_GE(A, B) UNIT_TEST_CHECK_OP(A, B, >=)


/// alias of UNIT_TEST_CHECK
#define UNIT_TEST_EXPECT UNIT_TEST_CHECK
#define UNIT_TEST_EXPECT_EQ  UNIT_TEST_CHECK_EQ
#define UNIT_TEST_EXPECT_NE  UNIT_TEST_CHECK_NE
#define UNIT_TEST_EXPECT_LT  UNIT_TEST_CHECK_LT
#define UNIT_TEST_EXPECT_LE  UNIT_TEST_CHECK_LE
#define UNIT_TEST_EXPECT_GT  UNIT_TEST_CHECK_GT
#define UNIT_TEST_EXPECT_GE  UNIT_TEST_CHECK_GE


/// do fatal check
#define UNIT_TEST_REQUIRE(Condition) \
        if (!(Condition)) \
        { \
            UnitTestCase::ReportFail(__FILE__, __LINE__, #Condition); \
        } \
        LEVEL_CHECK(FAIL, Condition, Condition)\

#define UNIT_TEST_REQUIRE_OP(A, B, OP)  \
        UNIT_TEST_REPORT_FAIL((A)OP(B), A OP B);\
        LEVEL_CHECK_OP(FAIL, A, B, OP)\

#define UNIT_TEST_REQUIRE_EQ(A, B) UNIT_TEST_REQUIRE_OP(A, B, ==)
#define UNIT_TEST_REQUIRE_NE(A, B) UNIT_TEST_REQUIRE_OP(A, B, !=)
#define UNIT_TEST_REQUIRE_LT(A, B) UNIT_TEST_REQUIRE_OP(A, B, < )
#define UNIT_TEST_REQUIRE_LE(A, B) UNIT_TEST_REQUIRE_OP(A, B, <=)
#define UNIT_TEST_REQUIRE_GT(A, B) UNIT_TEST_REQUIRE_OP(A, B, > )
#define UNIT_TEST_REQUIRE_GE(A, B) UNIT_TEST_REQUIRE_OP(A, B, >=)


class UnitTestCase
{
    struct Fail
    {
        const char* file;
        int line;
        const char* expr;
    };
public:
    UnitTestCase(const char* name, void (*routine)()):
        m_name(name), m_routine(routine), m_next(NULL)
    {
        if (Head() == NULL)
            Head() = this;

        if (Tail() == NULL)
            Tail() = Head();
        else
            Tail()->m_next = this;

        Tail() = this;
    }

    bool Run() const
    {
        bool result = false;

        try
        {
            fprintf(stderr, "[INFO]  Test %s start\n", m_name);
            m_routine();
            fprintf(stderr, "[INFO]  Test %s complete\n", m_name);
            result = true;
        }
        catch (UnitTestError& e)
        {
            TextColorSwitcher tcs(FOREGROUND_RED);
            fprintf(stderr, "%s\n", e.what());
            fprintf(stderr, "[FAIL]  TestCase %s\n", m_name);
        }
        printf("----------------------------------------------------------------------------\n");

        return result;
    }

    /// return error case count
    static int RunAll()
    {
        int error_count = 0;
        UnitTestCase* test_case = Head();

        while (test_case)
        {
            if (!test_case->Run())
                ++error_count;

            test_case = test_case->m_next;
        }

        const std::vector<Fail>& fails = Fails();

        if (!fails.empty())
        {
            fprintf(
                stderr,
                "============================================================================\n"
                "Summary:\n"
                "----------------------------------------------------------------------------\n"
            );

            TextColorSwitcher tcs(FOREGROUND_RED);
            fprintf(stderr, "Total %u errors found:\n", (unsigned int) fails.size());

            for (size_t i = 0; i < fails.size(); ++i)
            {
                fprintf(stderr, "[FAIL]  %s:%d %s\n", fails[i].file, fails[i].line, fails[i].expr);
            }
            return EXIT_FAILURE;
        }
        else
        {
            TextColorSwitcher tcs(FOREGROUND_GREEN);
            fprintf(stderr, "All pass\n");
            return EXIT_SUCCESS;
        }
    }
    static void ReportFail(const char* file, int line, const char* expr)
    {
        Fail fail = { file, line, expr };
        Fails().push_back(fail);
    }
    static std::vector<Fail>& Fails()
    {
        static std::vector<Fail> fails;
        return fails;
    }
private:
    static UnitTestCase*& Head()
    {
        static UnitTestCase* head;
        return head;
    }
    static UnitTestCase*& Tail()
    {
        static UnitTestCase* tail;
        return tail;
    }
private:
    const char* m_name;
    void (*m_routine)();
    UnitTestCase* m_next;
};

#define UNIT_TEST_CASE(Name) \
    static void Test_##Name(); \
    static UnitTestCase test_case_##Name(#Name, Test_##Name); \
    static void Test_##Name()

#define UNIT_TEST_RUN() UnitTestCase::RunAll()

#endif // UNIT_TEST_HPP
