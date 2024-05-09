#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <bitset>
#include <cassert>

class HuffmanNode
{
public:
   unsigned char name;
   int value;
   char bit;
   HuffmanNode *parent;

   HuffmanNode(unsigned char name, int value, char bit = '\0', HuffmanNode *parent = nullptr)
       : name(name), value(value), bit(bit), parent(parent) {}

   bool operator<(const HuffmanNode &obj) const
   {
      return this->value > obj.value;
   }
};

class Huffman
{
public:
   std::vector<unsigned char> encode_translation_table(std::map<unsigned char, std::string> &translation_table)
   {
      std::vector<unsigned char> encode;
      for (auto &pair : translation_table)
      {
         encode.push_back(pair.first);         // Store the character itself
         encode.push_back(pair.second.size()); // Store the size of the code
         for (char bit : pair.second)
         { // Store the code bits
            encode.push_back(bit);
         }
      }
      return encode;
   }

   std::map<unsigned char, std::string> decode_translation_table(std::vector<unsigned char> &translation_table)
   {
      std::map<unsigned char, std::string> reverse_translation_table;
      int i = 0;
      while (i < translation_table.size())
      {
         unsigned char key = translation_table[i++];
         int size = translation_table[i++];
         std::string code;
         for (int j = 0; j < size; ++j)
         {
            code += translation_table[i++];
         }
         reverse_translation_table[key] = code;
      }
      return reverse_translation_table;
   }

   std::vector<unsigned char> encode(const std::vector<unsigned char> &message)
   {
      std::priority_queue<HuffmanNode> pq;
      std::map<unsigned char, int> counts;
      std::map<unsigned char, HuffmanNode *> base_nodes;

      for (unsigned char byte : message)
      {
         counts[byte]++;
      }

      for (auto &count : counts)
      {
         HuffmanNode *huffman_node = new HuffmanNode(count.first, count.second);
         pq.push(*huffman_node);
         base_nodes[count.first] = huffman_node;
      }

      while (pq.size() > 1)
      {
         HuffmanNode *left_node = new HuffmanNode(pq.top().name, pq.top().value, '0');
         pq.pop();
         HuffmanNode *right_node = new HuffmanNode(pq.top().name, pq.top().value, '1');
         pq.pop();
         HuffmanNode *internal_node = new HuffmanNode('\0', left_node->value + right_node->value);
         left_node->parent = internal_node;
         right_node->parent = internal_node;
         pq.push(*internal_node);
      }

      std::map<unsigned char, std::string> translation;
      for (auto &pair : base_nodes)
      {
         std::string code = "";
         HuffmanNode *current = pair.second;
         while (current->parent)
         {
            code = current->bit + code;
            current = current->parent;
         }
         translation[pair.first] = code;
      }

      std::string encoded_message;
      for (unsigned char byte : message)
      {
         encoded_message += translation[byte];
      }

      std::vector<unsigned char> encoded_bytes;
      for (int i = 0; i < encoded_message.size(); i += 8)
      {
         std::bitset<8> bits(encoded_message.substr(i, 8));
         encoded_bytes.push_back(static_cast<unsigned char>(bits.to_ulong()));
      }

      std::vector<unsigned char> translation_table_bytes = encode_translation_table(translation);
      std::vector<unsigned char> result;
      result.push_back(static_cast<unsigned char>(translation_table_bytes.size()));
      result.insert(result.end(), translation_table_bytes.begin(), translation_table_bytes.end());
      result.insert(result.end(), encoded_bytes.begin(), encoded_bytes.end());

      return result;
   }

   std::vector<unsigned char> decode(const std::vector<unsigned char> &encoded)
   {
      int translation_table_size = encoded[0];
      std::map<unsigned char, std::string> translation_table;
      int index = 1;
      while (index <= translation_table_size)
      {
         unsigned char key = encoded[index++];
         int size = encoded[index++];
         std::string code;
         for (int j = 0; j < size; ++j)
         {
            code += encoded[index++];
         }
         translation_table[key] = code;
      }

      std::string encoded_message;
      for (int i = index; i < encoded.size(); ++i)
      {
         std::bitset<8> bits(encoded[i]);
         encoded_message += bits.to_string();
      }

      std::string decoded_message;
      std::string current_code;
      for (char bit : encoded_message)
      {
         current_code += bit;
         for (auto &pair : translation_table)
         {
            if (pair.second == current_code)
            {
               decoded_message += pair.first;
               current_code = "";
               break;
            }
         }
      }

      std::vector<unsigned char> result(decoded_message.begin(), decoded_message.end());
      return result;
   }
};

int main()
{
   std::ifstream file("./ProfileSm.tin", std::ios::binary);
   std::vector<unsigned char> raw((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

   Huffman huffman;
   std::vector<unsigned char> encoded = huffman.encode(raw);

   std::ofstream output_file("./ProfileSm.tin.hf", std::ios::binary);
   output_file.write(reinterpret_cast<char *>(encoded.data()), encoded.size());
   output_file.close();

   std::vector<unsigned char> decoded = huffman.decode(encoded);
   assert(raw == decoded);

   return 0;
}
