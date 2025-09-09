import java.util.*;
import java.io.*;
import java.nio.file.*;
import java.util.concurrent.ThreadLocalRandom;

import org.graphstream.graph.*;
import org.graphstream.graph.implementations.*;
//import org.graphstream.ui.swingViewer;
import org.graphstream.ui.swing_viewer.*;
import org.graphstream.ui.view.*;

public class GraphTextAnalyzer {
    private static Map<String, Map<String, Integer>> graph = new HashMap<>();
    private static Set<String> nodes = new HashSet<>();

    public static void main(String[] args) {
        String filePath = "./test/Easy Test.txt";
        if (args.length > 0) {
            filePath = args[0];
        }

        // 读取文本并生成有向图
        readTextAndBuildGraph(filePath);

        Scanner scanner = new Scanner(System.in);
        boolean running = true;

        while (running) {
            System.out.println("\n请选择操作:");
            System.out.println("1. 显示有向图");
            System.out.println("2. 查询桥接词");
            System.out.println("3. 生成新文本");
            System.out.println("4. 计算最短路径");
            System.out.println("5. 计算 PageRank");
            System.out.println("6. 随机游走");
            System.out.println("0. 退出");

            System.out.print("请输入选项: ");
            int choice = scanner.nextInt();
            scanner.nextLine(); // 消耗换行符

            switch (choice) {
                case 1:
                    displayGraphWithGraphStream();
                    break;
                case 2:
                    System.out.print("输入 word1: ");
                    String word1 = scanner.nextLine().toLowerCase();
                    System.out.print("输入 word2: ");
                    String word2 = scanner.nextLine().toLowerCase();
                    String bridgeResult = queryBridgeWords(word1, word2);
                    System.out.println(bridgeResult);
                    break;
                case 3:
                    System.out.print("输入新文本: ");
                    String inputText = scanner.nextLine();
                    String newText = generateNewText(inputText);
                    System.out.println("生成新的文本: " + newText);
                    break;
                case 4:
                    System.out.print("输入 word1: ");
                    String startWord = scanner.nextLine().toLowerCase();
                    System.out.print("输入 word2: ");
                    String endWord = scanner.nextLine().toLowerCase();
                    String pathResult = calcShortestPath(startWord, endWord);
                    System.out.println(pathResult);
                    break;
                case 5:
                    System.out.print("输入单词: ");
                    String prWord = scanner.nextLine().toLowerCase();
                    double pr = calPageRank(prWord);
                    System.out.printf("'%s' 的 PageRank 值为: %.4f\n", prWord, pr);
                    break;
                case 6:
                    String walkResult = randomWalk();
                    System.out.println("随机游走结果: " + walkResult);
                    break;
                case 0:
                    running = false;
                    break;
                default:
                    System.out.println("无效选项，请重新输入");
            }
        }

        scanner.close();
    }

    public static void readTextAndBuildGraph(String filePath) {
        try {
            List<String> lines = Files.readAllLines(Paths.get(filePath));
            StringBuilder textBuilder = new StringBuilder();

            // 合并所有行，将换行符和标点替换为空格
            for (String line : lines) {
                textBuilder.append(line.replaceAll("[^a-zA-Z\\s]", " ")).append(" ");
            }

            // 保留仅包含字母的单词，并转换为小写
            String[] words = textBuilder.toString().toLowerCase().split("\\s+");

            // 构建有向图
            for (int i = 0; i < words.length - 1; i++) {
                String currentWord = words[i];
                String nextWord = words[i + 1];

                if (!currentWord.isEmpty() && !nextWord.isEmpty()) {
                    nodes.add(currentWord);
                    nodes.add(nextWord);

                    graph.putIfAbsent(currentWord, new HashMap<>());
                    Map<String, Integer> edges = graph.get(currentWord);
                    edges.put(nextWord, edges.getOrDefault(nextWord, 0) + 1);
                }
            }

            System.out.println("图已成功生成!");
        } catch (IOException e) {
            System.err.println("读取文件时出错: " + e.getMessage());
        }
    }

    public static void showDirectedGraph() {
        if (nodes.isEmpty()) {
            System.out.println("图为空");
            return;
        }

        System.out.println("有向图结构:");
        for (String node : nodes) {
            if (graph.containsKey(node)) {
                Map<String, Integer> edges = graph.get(node);
                if (!edges.isEmpty()) {
                    System.out.print(node + " -> {");
                    StringJoiner joiner = new StringJoiner(", ");
                    for (Map.Entry<String, Integer> entry : edges.entrySet()) {
                        joiner.add(entry.getKey() + "(" + entry.getValue() + ")");
                    }
                    System.out.println(" " + joiner + " }");
                } else {
                    System.out.println(node + " (无出边)");
                }
            }
        }
    }

    public static void displayGraphWithGraphStream() {
        if (nodes.isEmpty()) {
            System.out.println("图为空");
            return;
        }

        // 创建图
        Graph graphStream = new SingleGraph("Text Graph");
        graphStream.setAttribute("ui.antialias");

        // 添加节点
        for (String node : nodes) {
            graphStream.addNode(node);
            graphStream.getNode(node).setAttribute("ui.label", node);
            graphStream.getNode(node).setAttribute("ui.size", 15f, 15f);
        }

        // 添加边
        for (Map.Entry<String, Map<String, Integer>> entry : graph.entrySet()) {
            String source = entry.getKey();
            for (Map.Entry<String, Integer> edge : entry.getValue().entrySet()) {
                String target = edge.getKey();
                int weight = edge.getValue();

                Edge edgeStream = graphStream.addEdge(source + "->" + target, source, target, true);
                edgeStream.setAttribute("ui.label", weight);
                edgeStream.setAttribute("weight", weight);
            }
        }

//        // 显示图
//        View view = new SwingViewer(graphStream, SwingViewer.ThreadingModel.GRAPH_IN_GUI_THREAD);
//        view.enableAutoLayout();
//        view.setView(new SingleGraphView());
//        view.openFrame();
    }

    public static String queryBridgeWords(String word1, String word2) {
        if (!nodes.contains(word1) || !nodes.contains(word2)) {
            return "No word1 or word2 in the graph!";
        }

        Set<String> bridgeWords = new HashSet<>();

        if (graph.containsKey(word1)) {
            for (Map.Entry<String, Integer> entry : graph.get(word1).entrySet()) {
                String candidate = entry.getKey();
                if (graph.containsKey(candidate) && graph.get(candidate).containsKey(word2)) {
                    bridgeWords.add(candidate);
                }
            }
        }

        if (bridgeWords.isEmpty()) {
            return "No bridge words from " + word1 + " to " + word2 + "!";
        } else {
            StringJoiner joiner = new StringJoiner(", ");
            for (String bridge : bridgeWords) {
                joiner.add(bridge);
            }
            return "The bridge words from " + word1 + " to " + word2 + " are: " + joiner;
        }
    }

    public static String generateNewText(String inputText) {
        if (nodes.isEmpty()) {
            return "图为空，无法生成新文本";
        }

        StringBuilder newText = new StringBuilder();
        String[] words = inputText.split("\\s+");

        for (int i = 0; i < words.length; i++) {
            String currentWord = words[i].toLowerCase();
            newText.append(currentWord);

            if (i < words.length - 1) {
                String nextWord = words[i + 1].toLowerCase();
                if (nodes.contains(currentWord) && nodes.contains(nextWord)) {
                    List<String> bridgeWords = getBridgeWords(currentWord, nextWord);
                    if (!bridgeWords.isEmpty()) {
                        String bridgeWord = bridgeWords.get(ThreadLocalRandom.current().nextInt(bridgeWords.size()));
                        newText.append(" ").append(bridgeWord);
                    }
                }
                newText.append(" ");
            }
        }

        return newText.toString().trim();
    }

    private static List<String> getBridgeWords(String word1, String word2) {
        List<String> bridgeWords = new ArrayList<>();

        if (graph.containsKey(word1)) {
            for (Map.Entry<String, Integer> entry : graph.get(word1).entrySet()) {
                String candidate = entry.getKey();
                if (graph.containsKey(candidate) && graph.get(candidate).containsKey(word2)) {
                    bridgeWords.add(candidate);
                }
            }
        }

        return bridgeWords;
    }

    public static String calcShortestPath(String word1, String word2) {
        if (!nodes.contains(word1) || !nodes.contains(word2)) {
            return "输入的单词不在图中";
        }

        // 使用 Dijkstra 算法计算最短路径
        Map<String, Double> distances = new HashMap<>();
        Map<String, String> previous = new HashMap<>();
        Set<String> unvisited = new HashSet<>(nodes);

        for (String node : nodes) {
            distances.put(node, Double.MAX_VALUE);
        }
        distances.put(word1, 0.0);

        while (!unvisited.isEmpty()) {
            // 在未访问节点中找到距离最小的节点
            String current = unvisited.stream()
                    .min(Comparator.comparingDouble(distances::get))
                    .orElse(null);

            if (current == null) {
                break;
            }

            if (current.equals(word2)) {
                break;
            }

            unvisited.remove(current);

            if (graph.containsKey(current)) {
                for (Map.Entry<String, Integer> entry : graph.get(current).entrySet()) {
                    String neighbor = entry.getKey();
                    double weight = entry.getValue();

                    double distance = distances.get(current) + weight;
                    if (distance < distances.get(neighbor)) {
                        distances.put(neighbor, distance);
                        previous.put(neighbor, current);
                    }
                }
            }
        }

        // 构建最短路径
        if (distances.get(word2) == Double.MAX_VALUE) {
            return "从 " + word1 + " 到 " + word2 + " 无路径";
        }

        StringBuilder path = new StringBuilder();
        String current = word2;
        while (current != null && !current.equals(word1)) {
            path.insert(0, " -> ").insert(0, current);
            current = previous.get(current);
        }
        if (current != null) {
            path.insert(0, word1);
        } else {
            return "从 " + word1 + " 到 " + word2 + " 无路径";
        }

        return String.format("从 %s 到 %s 的最短路径: %s (权重之和: %.2f)",
                word1, word2, path.toString(), distances.get(word2));
    }

    public static double calPageRank(String word) {
        if (!nodes.contains(word)) {
            return 0.0;
        }

        double d = 0.85; // 阻尼因子
        int maxIterations = 100;
        double minDelta = 0.00001;

        Map<String, Double> pr = new HashMap<>();
        Map<String, Double> newPr = new HashMap<>();

        // 初始化 PageRank 值
        double initialPr = 1.0 / nodes.size();
        for (String node : nodes) {
            pr.put(node, initialPr);
        }

        for (int i = 0; i < maxIterations; i++) {
            double diff = 0.0;
            for (String node : nodes) {
                double rank = (1.0 - d) / nodes.size();

                // 计算指向当前节点的所有节点的贡献
                for (Map.Entry<String, Map<String, Integer>> entry : graph.entrySet()) {
                    String source = entry.getKey();
                    Map<String, Integer> edges = entry.getValue();

                    if (edges.containsKey(node)) {
                        // 计算出边数量
                        int totalEdges = entry.getValue().size();
                        rank += d * pr.get(source) / totalEdges;
                    }
                }

                newPr.put(node, rank);
                diff += Math.abs(newPr.get(node) - pr.get(node));
            }

            // 检查是否收敛
            if (diff < minDelta) {
                break;
            }

            pr = new HashMap<>(newPr);
        }

        return pr.getOrDefault(word, 0.0);
    }

    public static String randomWalk() {
        if (nodes.isEmpty()) {
            return "图为空";
        }

        List<String> walkPath = new ArrayList<>();
        Set<String> visitedEdges = new HashSet<>();

        // 随机选择起点
        List<String> nodeList = new ArrayList<>(nodes);
        String current = nodeList.get(ThreadLocalRandom.current().nextInt(nodeList.size()));
        walkPath.add(current);

        while (graph.containsKey(current) && !graph.get(current).isEmpty()) {
            Map<String, Integer> edges = graph.get(current);
            List<String> nextNodes = new ArrayList<>(edges .keySet());

            if (nextNodes.isEmpty()) {
                break;
            }

            String nextNode = nextNodes.get(ThreadLocalRandom.current().nextInt(nextNodes.size()));
            String edge = current + "->" + nextNode;

            if (!visitedEdges.contains(edge)) {
                walkPath.add(nextNode);
                visitedEdges.add(edge);
                current = nextNode;
            } else {
                // 如果所有边都已访问过或没有出边，则停止
                break;
            }
        }

        return String.join(" -> ", walkPath);
    }
}