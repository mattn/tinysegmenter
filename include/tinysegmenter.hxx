#ifndef TINYSEGMENTER_HXX
#define TINYSEGMENTER_HXX

#include "weights.hxx"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace {
inline std::vector<char32_t> utf8_to_utf32(const std::string &utf8) {
  std::vector<char32_t> result;
  size_t i = 0;
  while (i < utf8.size()) {
    unsigned char c = static_cast<unsigned char>(utf8[i]);
    char32_t code_point;
    if ((c & 0x80) == 0) {
      code_point = c;
      i += 1;
    } else if ((c & 0xE0) == 0xC0) {
      code_point = ((c & 0x1F) << 6) | (utf8[i + 1] & 0x3F);
      i += 2;
    } else if ((c & 0xF0) == 0xE0) {
      code_point = ((c & 0x0F) << 12) | ((utf8[i + 1] & 0x3F) << 6) |
                   (utf8[i + 2] & 0x3F);
      i += 3;
    } else if ((c & 0xF8) == 0xF0) {
      code_point = ((c & 0x07) << 18) | ((utf8[i + 1] & 0x3F) << 12) |
                   ((utf8[i + 2] & 0x3F) << 6) | (utf8[i + 3] & 0x3F);
      i += 4;
    } else {
      ++i;
      continue;
    }
    result.push_back(code_point);
  }
  return result;
}

inline std::string utf32_to_utf8(char32_t c) {
  std::string result;
  if (c < 0x80) {
    result.push_back(static_cast<char>(c));
  } else if (c < 0x800) {
    result.push_back(static_cast<char>(0xC0 | (c >> 6)));
    result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
  } else if (c < 0x10000) {
    result.push_back(static_cast<char>(0xE0 | (c >> 12)));
    result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
    result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
  } else if (c < 0x110000) {
    result.push_back(static_cast<char>(0xF0 | (c >> 18)));
    result.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
    result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
    result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
  }
  return result;
}
} // namespace

class tinysegmenter {
public:
  using char_map = std::unordered_map<std::string, int>;

  inline tinysegmenter() : _BIAS(-332), preserve_tokens_(false) {
    init_weights();
  }

  inline std::vector<std::string> segment(const std::string &input) {
    if (input.empty())
      return {};
    if (preserve_tokens_)
      return segment_with_tokens(input);
    auto segments = segment_original(input);
    return merge_preserved_words(segments);
  }

  inline void set_preserve_list(const std::vector<std::string> &words) {
    preserve_list_ = words;
  }

  inline void set_preserve_tokens(bool enable) { preserve_tokens_ = enable; }

private:
  inline void init_weights() {
    _BC1 = {{"HH", 6}, {"II", 2461}, {"KH", 406}, {"OH", -1378}};
    _BC2 = {{"AA", -3267}, {"AI", 2744},  {"AN", -878},  {"HH", -4070},
            {"HM", -1711}, {"HN", 4012},  {"HO", 3761},  {"IA", 1327},
            {"IH", -1184}, {"II", -1332}, {"IK", 1721},  {"IO", 5492},
            {"KI", 3831},  {"KK", -8741}, {"MH", -3132}, {"MK", 3334},
            {"NM", 15000}, {"OO", -2920}};
    _BC3 = {{"HH", 996},  {"HI", 626},   {"HK", -721}, {"HN", -1307},
            {"HO", -836}, {"IH", -301},  {"KK", 2762}, {"MK", 1079},
            {"MM", 4034}, {"OA", -1652}, {"OH", 266}};
    _BP1 = {{"BB", 295}, {"OB", 304}, {"OO", -125}, {"UB", 352}};
    _BP2 = {{"BO", 60}, {"OO", -1762}};
    _BQ1 = BQ1;
    _BQ2 = BQ2;
    _BQ3 = BQ3;
    _BQ4 = BQ4;
    _BW1 = BW1;
    _BW2 = BW2;
    _BW3 = BW3;
    _TC1 = TC1;
    _TC2 = TC2;
    _TC3 = TC3;
    _TC4 = TC4;
    _TQ1 = TQ1;
    _TQ2 = TQ2;
    _TQ3 = TQ3;
    _TQ4 = TQ4;
    _TW1 = TW1;
    _TW2 = TW2;
    _TW3 = TW3;
    _TW4 = TW4;
    _UC1 = UC1;
    _UC2 = UC2;
    _UC3 = UC3;
    _UC4 = UC4;
    _UC5 = UC5;
    _UC6 = UC6;
    _UP1 = UP1;
    _UP2 = UP2;
    _UP3 = UP3;
    _UQ1 = UQ1;
    _UQ2 = UQ2;
    _UQ3 = UQ3;
    _UW1 = UW1;
    _UW2 = UW2;
    _UW3 = UW3;
    _UW4 = UW4;
    _UW5 = UW5;
    _UW6 = UW6;
    _NN = NN;
  }

  inline std::string ctype_rune(char32_t r) const {
    if ((r >= 0x4E00 && r <= 0x9FA0) || r == 0x3005 || r == 0x3006 ||
        r == 0x30F5 || r == 0x30F6)
      return "H";
    if (r >= 0x3041 && r <= 0x3093)
      return "I";
    if ((r >= 0x30A1 && r <= 0x30F4) || r == 0x30FC ||
        (r >= 0xFF71 && r <= 0xFF9D) || r == 0xFF9E || r == 0xFF70)
      return "K";
    if ((r >= 'a' && r <= 'z') || (r >= 'A' && r <= 'Z') ||
        (r >= 0xFF41 && r <= 0xFF5A) || (r >= 0xFF21 && r <= 0xFF3A))
      return "A";
    if ((r >= '0' && r <= '9') || (r >= 0xFF10 && r <= 0xFF19))
      return "N";
    if (r == U'\u4E00' || r == U'\u4E8C' || r == U'\u4E09' || r == U'\u56DB' ||
        r == U'\u4E94' || r == U'\u516D' || r == U'\u4E03' || r == U'\u516B' ||
        r == U'\u4E5D' || r == U'\u5341' || r == U'\u767E' || r == U'\u5343' ||
        r == U'\u4E07' || r == U'\u5104' || r == U'\u2085' || r == U'\u5146')
      return "M";
    return "O";
  }

  inline bool is_token_rune(char32_t r) const {
    return (r >= 'a' && r <= 'z') || (r >= 'A' && r <= 'Z') ||
           (r >= '0' && r <= '9') || r == '.' || r == '_' || r == '/' ||
           r == '\\' || r == '-' || r == ':' || r == '@';
  }

  inline std::vector<std::string>
  segment_original(const std::string &input) const {
    std::vector<std::string> result;
    std::vector<std::string> seg = {"B3", "B2", "B1"};
    std::vector<std::string> ctype = {"O", "O", "O"};

    auto utf32_runes = utf8_to_utf32(input);
    for (char32_t r : utf32_runes) {
      seg.push_back(utf32_to_utf8(r));
      ctype.push_back(ctype_rune(r));
    }
    seg.insert(seg.end(), {"E1", "E2", "E3"});
    ctype.insert(ctype.end(), {"O", "O", "O"});

    std::string word = seg[3];
    std::string p1 = "U", p2 = "U", p3 = "U";

    for (size_t i = 4; i < seg.size() - 3; ++i) {
      int score = _BIAS;
      std::string w1 = seg[i - 3], w2 = seg[i - 2], w3 = seg[i - 1];
      std::string w4 = seg[i], w5 = seg[i + 1], w6 = seg[i + 2];
      std::string c1 = ctype[i - 3], c2 = ctype[i - 2], c3 = ctype[i - 1];
      std::string c4 = ctype[i], c5 = ctype[i + 1], c6 = ctype[i + 2];

      score += get_value(_UP1, p1) + get_value(_UP2, p2) + get_value(_UP3, p3);
      score += get_value(_BP1, p1 + p2) + get_value(_BP2, p2 + p3);
      score += get_value(_UW1, w1) + get_value(_UW2, w2) + get_value(_UW3, w3) +
               get_value(_UW4, w4) + get_value(_UW5, w5) + get_value(_UW6, w6);
      score += get_value(_BW1, w2 + w3) + get_value(_BW2, w3 + w4) +
               get_value(_BW3, w4 + w5);
      score += get_value(_TW1, w1 + w2 + w3) + get_value(_TW2, w2 + w3 + w4) +
               get_value(_TW3, w3 + w4 + w5) + get_value(_TW4, w4 + w5 + w6);
      score += get_value(_UC1, c1) + get_value(_UC2, c2) + get_value(_UC3, c3) +
               get_value(_UC4, c4) + get_value(_UC5, c5) + get_value(_UC6, c6);
      score += get_value(_BC1, c2 + c3) + get_value(_BC2, c3 + c4) +
               get_value(_BC3, c4 + c5);
      score += get_value(_TC1, c1 + c2 + c3) + get_value(_TC2, c2 + c3 + c4) +
               get_value(_TC3, c3 + c4 + c5) + get_value(_TC4, c4 + c5 + c6);
      score += get_value(_UQ1, p1 + c1) + get_value(_UQ2, p2 + c2) +
               get_value(_UQ3, p3 + c3);
      score += get_value(_BQ1, p2 + c2 + c3) + get_value(_BQ2, p2 + c3 + c4) +
               get_value(_BQ3, p3 + c2 + c3) + get_value(_BQ4, p3 + c3 + c4);
      score += get_value(_TQ1, p2 + c1 + c2 + c3) +
               get_value(_TQ2, p2 + c2 + c3 + c4) +
               get_value(_TQ3, p3 + c1 + c2 + c3) +
               get_value(_TQ4, p3 + c2 + c3 + c4);
      score += get_value(_NN, c3 + c4);

      std::string p = "O";
      if (score > 0) {
        result.push_back(word);
        word.clear();
        p = "B";
      }
      p1 = p2;
      p2 = p3;
      p3 = p;
      word += seg[i];
    }
    result.push_back(word);
    return result;
  }

  inline std::vector<std::string>
  merge_preserved_words(const std::vector<std::string> &segments) const {
    auto result = segments;
    for (const auto &preserve : preserve_list_) {
      result = merge_if_matches(result, preserve);
    }
    return result;
  }

  inline std::vector<std::string>
  merge_if_matches(const std::vector<std::string> &segments,
                   const std::string &target) const {
    std::vector<std::string> result;
    size_t i = 0;
    while (i < segments.size()) {
      size_t length = 0;
      if (try_match(segments, i, target, length)) {
        result.push_back(target);
        i += length;
      } else {
        result.push_back(segments[i]);
        ++i;
      }
    }
    return result;
  }

  inline bool try_match(const std::vector<std::string> &segments, size_t pos,
                        const std::string &target, size_t &length) const {
    std::string combined;
    for (size_t i = pos; i < segments.size(); ++i) {
      combined += segments[i];
      if (combined == target) {
        length = i - pos + 1;
        return true;
      }
      if (combined.length() >= target.length())
        break;
    }
    return false;
  }

  inline std::vector<std::string>
  segment_with_tokens(const std::string &input) const {
    std::vector<std::string> result;
    size_t last_end = 0;
    auto utf32_runes = utf8_to_utf32(input);
    size_t i = 0;
    while (i < utf32_runes.size()) {
      if (is_token_rune(utf32_runes[i])) {
        size_t start = i;
        while (i < utf32_runes.size() && is_token_rune(utf32_runes[i]))
          ++i;
        if (start > last_end) {
          std::string before_text;
          for (size_t j = last_end; j < start; ++j)
            before_text += utf32_to_utf8(utf32_runes[j]);
          auto segments = segment_original(before_text);
          auto merged = merge_preserved_words(segments);
          result.insert(result.end(), merged.begin(), merged.end());
        }
        std::string token;
        for (size_t j = start; j < i; ++j)
          token += utf32_to_utf8(utf32_runes[j]);
        result.push_back(token);
        last_end = i;
      } else {
        ++i;
      }
    }
    if (last_end < utf32_runes.size()) {
      std::string remaining_text;
      for (size_t j = last_end; j < utf32_runes.size(); ++j)
        remaining_text += utf32_to_utf8(utf32_runes[j]);
      auto segments = segment_original(remaining_text);
      auto merged = merge_preserved_words(segments);
      result.insert(result.end(), merged.begin(), merged.end());
    }
    return result;
  }

  static inline int get_value(const char_map &map, const std::string &key) {
    auto it = map.find(key);
    return it != map.end() ? it->second : 0;
  }

  int _BIAS;
  char_map _BC1, _BC2, _BC3, _BP1, _BP2, _BQ1, _BQ2, _BQ3, _BQ4;
  char_map _BW1, _BW2, _BW3, _TC1, _TC2, _TC3, _TC4;
  char_map _TQ1, _TQ2, _TQ3, _TQ4, _TW1, _TW2, _TW3, _TW4;
  char_map _UC1, _UC2, _UC3, _UC4, _UC5, _UC6;
  char_map _UP1, _UP2, _UP3, _UQ1, _UQ2, _UQ3;
  char_map _UW1, _UW2, _UW3, _UW4, _UW5, _UW6, _NN;
  std::vector<std::string> preserve_list_;
  bool preserve_tokens_;
};

#endif
