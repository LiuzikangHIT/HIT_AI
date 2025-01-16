query0 = "SELECT [ ENAME = 'Mary' & DNAME = 'Research' ] ( EMPLOYEE JOIN DEPARTMENT )"
query1 = "PROJECTION [ BDATE ] ( SELECT [ ENAME = 'John' & DNAME = 'Research' ] ( EMPLOYEE JOIN DEPARTMENT ) )"
query2 = "SELECT [ ESSN = '01' ] ( PROJECTION [ ESSN, PNAME ] ( WORKS_ON JOIN PROJECT ) )"

queries = [query0, query1, query2]

# 查询树节点
class QueryTreeNode:
    def __init__(self, op='', info=''):
        self.child = []     # 子节点
        self.op = op        # 操作符
        self.info = info    # 条件

    def __str__(self):
        return (self.op if self.op else '') + (' ' + self.info if self.info else '') # 当作为字符串使用时，输出操作符和条件


def get_tree(query: str):
    tokens, idx, node = query.split(), 0, QueryTreeNode()
    while idx < len(tokens):
        token = tokens[idx]
        if token == 'SELECT' or token == 'PROJECTION':
            end = tokens.index(']', idx)    # 找到最近的]的索引
            node.op, node.info = token, ' '.join(tokens[idx + 2:end])   # info存入选择条件
            idx = end + 1
        elif token == 'JOIN':
            node.op = token
            node.child.append(QueryTreeNode(info=tokens[idx - 1]))  # 连接操作的第一个关系
            node.child.append(QueryTreeNode(info=tokens[idx + 1]))  # 连接操作的第二个关系
            idx += 1
        elif token == '(':  # 括号内为查询子句
            count, idy = 1, idx + 1
            while idy < len(tokens) and count > 0:
                if tokens[idy] == '(':
                    count += 1
                elif tokens[idy] == ')':
                    count -= 1
                idy += 1
            node.child.append(get_tree(' '.join(tokens[idx + 1:idy - 1])))  # 递归调用，并将结果加入上层子节点
            idx = idy
        else:
            idx += 1
    return node


def output_tree(node: QueryTreeNode, sep=''):
    print(sep + str(node))
    if len(node.child) >= 1:
        output_tree(node.child[0], sep + '\t')
    if len(node.child) >= 2:
        output_tree(node.child[1], sep + '\t')

# 查询优化
def optimize(node: QueryTreeNode, info_lst=None) -> QueryTreeNode:
    # 遇到选择和投影时，记录下二者条件，并递归优化其子树
    if node.op == 'SELECT':
        node = optimize(node.child[0], node.info.split('&'))
    elif node.op == 'PROJECTION':
        node.child[0] = optimize(node.child[0], info_lst)
    # 遇到连接时，将上层的选择操作下推（投影不下推）
    elif node.op == 'JOIN':
        node0 = QueryTreeNode(op='SELECT', info=info_lst[0])
        node0.child.append(node.child[0])
        node.child[0] = node0
        if len(info_lst) > 1:
            node1 = QueryTreeNode(op='SELECT', info=info_lst[1])
            node1.child.append(node.child[1])
            node.child[1] = node1
    return node


if __name__ == '__main__':
    print(query0, "\n\nOrigianl query1:")
    tree1 = get_tree(query0)
    output_tree(tree1)
    print("\nOptimized query1:")
    tree1 = optimize(tree1)
    output_tree(tree1)
    print("=" * 100)

    print(query1, "\n\nOrigianl query2:")
    tree2 = get_tree(query1)
    output_tree(tree2)
    print("\nOptimized query2:")
    tree2 = optimize(tree2)
    output_tree(tree2)
    print("=" * 100)

    print(query2, "\n\nOrigianl query3:")
    tree3 = get_tree(query2)
    output_tree(tree3)
    print("\nOptimized query3:")
    tree3 = optimize(tree3)
    output_tree(tree3)
