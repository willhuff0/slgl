#include <slgl/runtime/game/Node.hpp>

#include <memory>
#include <utility>

struct NodeComponent {
public:
    explicit NodeComponent(std::shared_ptr<Node> node) : node(std::move(node)) { }

    [[nodiscard]] const Node* GetNode() const { return node.get(); }
    [[nodiscard]] Node* GetNode()  { return node.get(); }

private:
    std::shared_ptr<Node> node;
};
