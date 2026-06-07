#include "pch.h";
#include "trie.hpp"
#include "id3.hpp"

namespace ID3 {
	trie::Trie ID3v2::frameTags({ { '0', '9' }, { 'A', 'Z' } });
}