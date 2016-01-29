/**
 * @file check.h
 * @brief
 * @author kypoyin, kypoyin@tencent.com
 * @date 2009-10-29
 */

#ifndef  COMMON_LEVEL_CHECK_H_
#define  COMMON_LEVEL_CHECK_H_

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <cstddef>
#include <string>
#include <string.h>

enum LogLevel{

    LOGLEVEL_INFO,
    LOGLEVEL_WARNING,
    LOGLEVEL_ERROR,
    LOGLEVEL_FATAL,

#ifdef NDEBUG
    LOGLEVEL_DFATAL = LOGLEVEL_ERROR,
#else
    LOGLEVEL_DFATAL = LOGLEVEL_FATAL,
#endif

    LOGLEVEL_FAIL = LOGLEVEL_ERROR,
    LOGLEVEL_ = LOGLEVEL_INFO
};


class LogFinisher;
class LogMessage;

class LogMessage {

  public:
    LogMessage(const char* filename, int line, LogLevel level, bool  isok = true, std::string expression = "");
    ~LogMessage();


    LogMessage& operator<<(const std::string& value);
    LogMessage& operator<<(const char* value);
    LogMessage& operator<<(bool value);
    LogMessage& operator<<(char value);
    LogMessage& operator<<(int value);
    LogMessage& operator<<(unsigned int value);
    LogMessage& operator<<(long value);
    LogMessage& operator<<(unsigned long value);
    LogMessage& operator<<(long long value);
    LogMessage& operator<<(unsigned long long value);
    LogMessage& operator<<(double value);
    LogMessage& operator<<(void  *value);

  private:
    friend class LogFinisher;
    void Finish();

  private:
    LogLevel                m_level;
    bool                    m_ok;
    std::string             m_message;
    const char*             m_filename;
    int                     m_line;
};

class  LogFinisher
{
public:
    LogFinisher& operator=(LogMessage& other);
};

template<typename T>
LogMessage& operator<< (LogMessage& out, T& m)
{
    return out << (void *)&m ;
}

#define DEFAINE_EXTERN_CHECK_TYPE(TYPE, val, out) LogMessage& operator<<(LogMessage& out,  TYPE val)

template <class T>
inline const T&             GetReferenceableValue_(const T&           t) { return t; }
inline char                 GetReferenceableValue_(char               t) { return t; }
inline unsigned char        GetReferenceableValue_(unsigned char      t) { return t; }
inline signed char          GetReferenceableValue_(signed char        t) { return t; }
inline int16_t              GetReferenceableValue_(int16_t            t) { return t; }
inline uint16_t             GetReferenceableValue_(uint16_t           t) { return t; }
inline int32_t              GetReferenceableValue_(int32_t            t) { return t; }
inline uint32_t             GetReferenceableValue_(uint32_t           t) { return t; }
inline long                 GetReferenceableValue_(long               t) { return t; }
inline unsigned long        GetReferenceableValue_(unsigned long      t) { return t; }
inline long long            GetReferenceableValue_(long long          t) { return t; }
inline unsigned long long   GetReferenceableValue_(unsigned long long t) { return t; }
inline float                GetReferenceableValue_(float              f) { return f; }
inline double               GetReferenceableValue_(double            lf) { return lf;}

#undef LEVEL_CHECK_LOG
#undef LEVEL_CHECK_LOG_IF

#undef CHECK
#undef LEVEL_CHECK_EQ
#undef LEVEL_CHECK_NE
#undef LEVEL_CHECK_LT
#undef LEVEL_CHECK_LE
#undef LEVEL_CHECK_GT
#undef LEVEL_CHECK_GE

#undef DLOG
#undef DCHECK
#undef DLEVEL_CHECK_EQ
#undef DLEVEL_CHECK_NE
#undef DLEVEL_CHECK_LT
#undef DLEVEL_CHECK_LE
#undef DLEVEL_CHECK_GT
#undef DLEVEL_CHECK_GE

#define LEVEL_CHECK_LOG(...)        \
        LogFinisher() = LogMessage( __FILE__, __LINE__, LOGLEVEL_##__VA_ARGS__)

#define LEVEL_CHECK_LOG_IF(LEVEL, CONDITION, EXPRESSION) \
        LogFinisher() = (\
              !(CONDITION) ? LogMessage(__FILE__, __LINE__, LOGLEVEL_INFO, true, "("#EXPRESSION")")\
                      : LogMessage(__FILE__, __LINE__, LOGLEVEL_##LEVEL, false, "(" #EXPRESSION")"))

#define LEVEL_CHECK(LEVEL, CONDITION, EXPRESSION) \
    LEVEL_CHECK_LOG_IF(LEVEL, !(CONDITION), EXPRESSION ) << " "


#define  PAIR_VALUE(A, B)           \
     "[ "#A" = " << GetReferenceableValue_(A) << "; "#B" = " << GetReferenceableValue_(B) << "]"

#define LEVEL_CHECK_OP(LEVEL, A, B, OP)  LEVEL_CHECK(LEVEL, (A) OP (B), A OP B) << PAIR_VALUE(A, B)
#define LEVEL_CHECK_EQ(LEVEL, A, B) LEVEL_CHECK_OP(LEVEL, A, B, ==)
#define LEVEL_CHECK_NE(LEVEL, A, B) LEVEL_CHECK_OP(LEVEL, A, B, !=)
#define LEVEL_CHECK_LT(LEVEL, A, B) LEVEL_CHECK_OP(LEVeL, A, B, < )
#define LEVEL_CHECK_LE(LEVEL, A, B) LEVEL_CHECK_OP(LEVEL, A, B, <=)
#define LEVEL_CHECK_GT(LEVEL, A, B) LEVEL_CHECK_OP(LEVEL, A, B, > )
#define LEVEL_CHECK_GE(LEVEL, A, B) LEVEL_CHECK_OP(LEVeL, A, B, >=)

#ifdef NDEBUG

#define DLEVEL_CHECK_LOG(...)   while(false) LEVEL_CHECK_LOG(__VA_ARGS__)
#define DLEVEL_CHECK(EXPRESSION) while(false) LEVEL_CHECK(EXPRESSION)
#define DLEVEL_CHECK_EQ(A, B) DLEVEL_CHECK((A) == (B))
#define DLEVEL_CHECK_NE(A, B) DLEVEL_CHECK((A) != (B))
#define DLEVEL_CHECK_LT(A, B) DLEVEL_CHECK((A) <  (B))
#define DLEVEL_CHECK_LE(A, B) DLEVEL_CHECK((A) <= (B))
#define DLEVEL_CHECK_GT(A, B) DLEVeL_CHECK((A) >  (B))
#define DLEVEL_CHECK_GE(A, B) DLEVEL_CHECK((A) >= (B))

#else  ///NDEBUG

#define DLEVEL_CHECK_LOG     LEVEL_CHECK_LOG

#define DLEVEL_CHECK    LEVEL_CHECK
#define DLEVEL_CHECK_EQ LEVEL_CHECK_EQ
#define DLEVEL_CHECK_NE LEVEL_CHECK_NE
#define DLEVEL_CHECK_LT LEVEL_CHECK_LT
#define DLEVEL_CHECK_LE LEVEL_CHECK_LE
#define DLEVEL_CHECK_GT LEVEL_CHECK_GT
#define DLEVEL_CHECK_GE LEVEL_CHECK_GE

#endif  ///!NDEBUG


typedef void LogHandler( bool passed,
                const char* filename, int line, const std::string& message);

/**
 * @brief SetLogHandler 设置用户自定义的Log输出函数
 *
 * @param new_handle_func user-defined logging function point
 *
 * @return
 */
LogHandler*  SetLogHandler(LogHandler* new_handle_func);


class  CheckSilencer {
    public:
        CheckSilencer();
        ~CheckSilencer();
};

#define DECLARE_CHECK_SILENCER( Silencer ) CheckSilencer  Silencer


void ShutdownALL();
void OnShutdown(void (*func)());

#endif  ///end defined

