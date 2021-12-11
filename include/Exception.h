#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <ctime>
#include <exception>
#include <string>
#include <string_view>

class Exception : public std::exception {

public:
  Exception(const std::string_view message = "Undefined exception")
      : std::exception() {

    time_t t = std::time(nullptr);

    struct tm *tm_s = std::localtime(&t);

    char str_t[32];

    std::strftime(str_t, sizeof(str_t), "%Y%m%d-%X - EXCEPTION: ", tm_s);

    _message = std::string(str_t).append(message);
  }

  ~Exception() throw() {}

  const char *what() const throw() override;

private:
  std::string _message;
};

inline const char *Exception::what() const throw() {

  return (_message.c_str());
}

#endif /* EXCEPTION_H_ */