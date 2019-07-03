import re
import string
from typing import Dict

alphabet = '{}{}{}{}'.format(string.digits, string.ascii_letters, '/', '-')

char_map = list(-1 for _ in range(128))


def initial_char_map():
    global char_map
    for idx in range(len(alphabet)):
        char_map[ord(alphabet[idx])] = idx


class TrieRgxNode:

    def __init__(self):
        self.nodes: Dict[int, 'TrieRgxNode'] = {}
        self.rgx_nodes = []
        self.rgx = ''
        self.value = -1

    def insert(self, word, val):
        current = self
        idx_word = 0
        len_word = len(word)
        while idx_word < len_word:
            current_char = word[idx_word]
            if current_char == '<':
                rgx_start_idx = idx_word + 1
                rgx_end_idx = rgx_start_idx
                while rgx_end_idx < len_word:
                    if word[rgx_end_idx] == '>':
                        break
                    rgx_end_idx += 1
                else:
                    raise RuntimeError("invalid insert word: {}".format(word))
                current.is_leaf = False
                rgx_char = word[rgx_start_idx: rgx_end_idx]
                rgx_node = None
                exist = False
                for exist_rgx_node in current.rgx_nodes:
                    if exist_rgx_node.rgx == rgx_char:
                        rgx_node = exist_rgx_node
                        exist = True
                if not exist:
                    rgx_node = TrieRgxNode()
                    rgx_node.rgx = rgx_char
                    current.rgx_nodes.append(rgx_node)
                current = rgx_node
                idx_word = rgx_end_idx + 1
                continue
            elif char_map[ord(current_char)] != -1:
                char_idx = char_map[ord(current_char)]
                next_node = current.nodes.get(char_idx) or TrieRgxNode()
                current.nodes[char_idx] = next_node
                current = next_node
                idx_word += 1
                continue
            else:
                raise RuntimeError("invalid word character: {} in {}".format(current_char, word))
        current.value = val

    def search(self, word):
        len_word = len(word)
        stack = list()
        stack.append(0)
        stack.append(self)
        while stack:
            node = stack.pop()
            idx_word = stack.pop()
            if idx_word == len_word:
                if node.value != -1:
                    return node.value
                else:
                    continue
            current_char = word[idx_word]
            current_char_idx = char_map[ord(current_char)]
            if current_char_idx == -1:
                raise RuntimeError('invalid word character: {} in {}'.format(current_char, word))
            if node.nodes.get(current_char_idx):
                stack.append(idx_word + 1)
                stack.append(node.nodes[current_char_idx])
            if node.rgx_nodes:
                for rgx_node in node.rgx_nodes:
                    match = re.match(rgx_node.rgx, word, idx_word)
                    if match:
                        stack.append(match.end())
                        stack.append(rgx_node)
                        break
        return -1


TrieRgxTree = TrieRgxNode

if __name__ == '__main__':
    initial_char_map()
    tree = TrieRgxTree()
    tree.insert(r'12def/ghi', 20)
    tree.insert(r'12<\d+>', 10)
    assert tree.search('12456') == 10
    assert tree.search('12def/ghi') == 20
