# dictmatch

关键词匹配，多模式匹配的c/c++实现。

# 什么是多模式匹配
在一段文本中同时匹配N个关键词。 最原始的实现，可以strstr查找N遍，但效率低下。

# 多模式匹配与分词的异同
多模式匹配主算法很像分词，但也有很多不同。
* 切分的结果不同：分词要求不仅仅切分出词典中有的关键词，还要切分出未登录词和不在词典中的单字。而多模式匹配，只要求切分出词典中有的关键词。
* 运算速度不同：多模式匹配库因为少了歧义消解，未登录词识别等很多复杂逻辑，计算速度比分词快两个数量级。
* 代码复杂度不同：通常代码量只是一套成熟分词系统的1/10.

