#pragma once

// Need std=c++17 or higher.
#if (__cplusplus > 201402L && __has_include(<optional>))
  #include <optional>
#else
  #include <experimental/optional>

  namespace std
  {

  template <typename T>
  using optional = std::experimental::optional<T>;

  }
#endif
