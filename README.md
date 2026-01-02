# tinysegmenter

C++ port of TinySegmenter 0.2. A lightweight header-only library for segmenting Japanese text into words.

This implementation add following features:

* the NN and BC2/NM feature addition from the original, which prevents consecutive numbers from being split.
* preserve strings that should not be segmented.
* preserve token that should be kept URL/E-mail as a single token.

## Installation

Simply include the header file in your project:

```cpp
#include "tinysegmenter.hxx"
```

## Usage

```cpp
#include <iostream>
#include "tinysegmenter.hxx"

int main() {
    tinysegmenter ts;
    auto result = ts.segment("私の名前は中野です");
    
    for (const auto& word : result) {
        std::cout << "[" << word << "] ";
    }
    std::cout << std::endl;
    // Output: [私] [の] [名前] [は] [中野] [です]
    
    return 0;
}
```

### Preserve List

```cpp
tinysegmenter ts;
ts.set_preserve_list({"東京都", "大阪府"});
auto result = ts.segment("東京都に住んでいます");
// Output: [東京都] [に] [住ん] [で] [い] [ます]
```

### Preserve Tokens

```cpp
tinysegmenter ts;
ts.set_preserve_tokens(true);
auto result = ts.segment("メールはtest@example.comです");
// Output: [メール] [は] [test@example.com] [です]
```

## Building

```bash
make all
make test
```

## Requirements

- C++17 or later
- Standard library only (no external dependencies)

## License

Modified BSD License (same as original TinySegmenter)

## Original Implementation

- [TinySegmenter](http://chasen.org/~taku/software/TinySegmenter/) by Taku Kudo

## Author

Yasuhiro Matsumoto (a.k.a mattn)
