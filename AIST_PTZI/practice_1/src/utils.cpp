#include "utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>

void print_number(const char *label, const mpz_t num)
{
  std::cout << label << ": " << mpz_get_str(nullptr, 16, num) << std::endl;
}

/*! Center-aligns string within a field of width w. Pads with blank spaces
    to enforce alignment. */
std::string center(const std::string s, const int w)
{
  std::stringstream ss, spaces;
  int padding = w - s.size(); // count excess room to pad
  for (int i = 0; i < padding / 2; ++i)
    spaces << " ";
  ss << spaces.str() << s << spaces.str(); // format with padding
  if (padding > 0 && padding % 2 != 0)     // if odd #, add 1 space
    ss << " ";
  return ss.str();
}

/*! Left-aligns string within a field of width w. Pads with blank spaces
    to enforce alignment. */
std::string left(const std::string s, const int w)
{
  std::stringstream ss, spaces;
  int padding = w - s.size(); // count excess room to pad
  for (int i = 0; i < padding; ++i)
    spaces << " ";
  ss << s << spaces.str(); // format with padding
  return ss.str();
}

/* Convert double to string with specified number of places after the decimal
   and left padding. */
std::string prd(const unsigned int x, const int width)
{
  std::stringstream ss;
  ss << std::right;
  ss.fill(' ');    // fill space around displayed #
  ss.width(width); // set  width around displayed #
  ss << x;
  return ss.str();
}