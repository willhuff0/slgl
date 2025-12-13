#include <slgl/runtime/game/Node.hpp>

#include <stdexcept>

glm::vec3 Node::GetLPos() const { return lPos; }
glm::quat Node::GetLRot() const { return lRot; }
glm::vec3 Node::GetLAnglesRad() const { return glm::eulerAngles(lRot); }
glm::vec3 Node::GetLAnglesDeg() const { return glm::degrees(glm::eulerAngles(lRot)); }
glm::vec3 Node::GetLScale() const { return lScale; }

void Node::SetLPos(glm::vec3 lp) {
    lPos = lp;
    SetDirty();
}
void Node::SetLRot(glm::quat lr) {
    lRot = lr;
    SetDirty();
}
void Node::SetLAnglesRad(glm::vec3 la) { SetLRot(glm::quat(la)); }
void Node::SetLAnglesDeg(glm::vec3 la) { SetLRot(glm::quat(glm::radians(la))); }
void Node::SetLScale(glm::vec3 ls) {
    lScale = ls;
    SetDirty();
}

glm::vec3 Node::GetWPos() const {
    if (!IsDirty()) return wPos;

    if (parent != nullptr) {
        return static_cast<const Node*>(parent.get())->GetWPos() * lPos;
    } else {
        return lPos;
    }
}
glm::quat Node::GetWRot() const {
    if (!IsDirty()) return wRot;

    if (parent != nullptr) {
        return static_cast<const Node*>(parent.get())->GetWRot() * lRot;
    } else {
        return lRot;
    }
}
glm::vec3 Node::GetWAnglesRad() const { return glm::eulerAngles(GetWRot()); }
glm::vec3 Node::GetWAnglesDeg() const { return glm::degrees(glm::eulerAngles(GetWRot())); }
glm::vec3 Node::GetWScale() const {
    if (!IsDirty()) return wScale;

    if (parent != nullptr) {
        return static_cast<const Node*>(parent.get())->GetWScale() * lScale;
    } else {
        return lScale;
    }
}

glm::vec3 Node::GetWPos() {
    if (IsDirty()) RecalculateWorld();
    return wPos;
}
glm::quat Node::GetWRot() {
    if (IsDirty()) RecalculateWorld();
    return wRot;
}
glm::vec3 Node::GetWAnglesRad() { return glm::eulerAngles(GetWRot()); }
glm::vec3 Node::GetWAnglesDeg() { return glm::degrees(glm::eulerAngles(GetWRot())); }
glm::vec3 Node::GetWScale() {
    if (IsDirty()) RecalculateWorld();
    return wScale;
}

void Node::SetWPos(glm::vec3 wp) {
    // Immediately calculate local
    if (parent != nullptr) {
        lPos = wp - parent->GetWPos();
    } else {
        // Root CNode
        lPos = wp;
    }

    if (!dirty) {
        SetDirty(); // Sets children dirty
        dirty = false;
    }

    wPos = wp;
}
void Node::SetWRot(glm::quat wr) {
    // Immediately calculate local
    if (parent != nullptr) {
        lRot = wr * glm::conjugate(parent->GetWRot());
    } else {
        // Root CNode
        lRot = wr;
    }

    if (!dirty) {
        SetDirty(); // Sets children dirty
        dirty = false;
    }

    wRot = wr;
}
void Node::SetWAnglesRad(glm::vec3 war) { SetWRot(glm::quat(war)); }
void Node::SetWAnglesDeg(glm::vec3 was) { SetWRot(glm::quat(glm::radians(was))); }
void Node::SetWScale(glm::vec3 ws) {
    // Immediately calculate local
    if (parent != nullptr) {
        lScale = ws / parent->GetWScale();
    } else {
        // Root CNode
        lScale = ws;
    }

    if (!dirty) {
        SetDirty(); // Sets children dirty
        dirty = false;
    }

    wScale = ws;
}

glm::mat4 Node::GetMatrix() const {
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), GetWPos());
    glm::mat4 rotation = glm::mat4_cast(GetWRot());
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), GetWScale());

    return translation * rotation * scale;
}
glm::mat4 Node::GetMatrix() {
    if (IsDirty()) RecalculateWorld();

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), wPos);
    glm::mat4 rotation = glm::mat4_cast(wRot);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), wScale);

    return translation * rotation * scale;
}
void Node::SetMatrix(glm::mat4 matrix) {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 position;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(matrix, scale, rotation, position, skew, perspective);
    SetWScale(scale);
    SetWRot(rotation);
    SetWPos(position);
}

bool Node::IsDirty() const { return dirty; }
void Node::SetDirty() {
    if (dirty) return; // All children have already been marked as well, so no need to recurse

    dirty = true;

    for (const std::shared_ptr<Node>& child : children) {
        child->SetDirty();
    }
}
void Node::RecalculateWorld() {
    if (parent != nullptr) {
        wPos = parent->GetWPos() + lPos;
        wRot = parent->GetWRot() * lRot;
        wScale = parent->GetWScale() * lScale;
    } else {
        // Root Node

        wPos = lPos;
        wRot = lRot;
        wScale = lScale;
    }

    dirty = false;
    // call change callbacks
}

void Node::AddChangeCallback(std::function<void(const std::shared_ptr<Node>&)> onChange) {
    changeCallbacks.emplace_back(std::move(onChange));
}

std::shared_ptr<Node> Node::Create(std::shared_ptr<Node> parent) {
    return std::shared_ptr<Node>(new Node(std::move(parent)), [](Node* ptr) { delete ptr; });
}

Node::Node(std::shared_ptr<Node> parent) : parent(std::move(parent)) {
    RecalculateWorld();
}

const std::unordered_set<std::shared_ptr<Node>>& Node::GetChildren() const { return children; }
size_t Node::GetChildCount() const { return children.size(); }

std::shared_ptr<Node> Node::CreateChild() {
    auto child = Node::Create(shared_from_this());
    children.emplace(child);
    return child;
}

void Node::AttachChild(const std::shared_ptr<Node>& target) {
    if (target->parent != nullptr) target->parent->DetachChild(target);
    target->parent = shared_from_this();
    children.emplace(target);

    target->SetDirty();
    target->RecalculateWorld();
}

void Node::DetachChild(const std::shared_ptr<Node>& target) {
    if (children.erase(target) == 0) throw std::runtime_error("target is not a child of this node!");
    target->parent = nullptr;

    target->SetDirty();
    target->RecalculateWorld();
}

void Node::DetachAllChildren() {
    for (const auto& child : children) {
        child->parent = nullptr;

        child->SetDirty();
        child->RecalculateWorld();
    }
    children.clear();
}

bool Node::IsEnabled() const { return isEnabled; }
void Node::SetIsEnabled(bool newIsEnabled) { isEnabled = newIsEnabled; }
