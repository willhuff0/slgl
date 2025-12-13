#pragma once

#include <slgl/common/Math.hpp>

#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <functional>

class Node : public std::enable_shared_from_this<Node> {
public:
    static std::shared_ptr<Node> Create(std::shared_ptr<Node> parent = nullptr);

    //#region Transform
    [[nodiscard]] glm::vec3 GetLPos() const;
    [[nodiscard]] glm::quat GetLRot() const;
    [[nodiscard]] glm::vec3 GetLAnglesRad() const;
    [[nodiscard]] glm::vec3 GetLAnglesDeg() const;
    [[nodiscard]] glm::vec3 GetLScale() const;

    void SetLPos(glm::vec3 lp);
    void SetLRot(glm::quat lr);
    void SetLAnglesRad(glm::vec3 lar);
    void SetLAnglesDeg(glm::vec3 lad);
    void SetLScale(glm::vec3 ls);

    [[nodiscard]] glm::vec3 GetWPos() const;
    [[nodiscard]] glm::quat GetWRot() const;
    [[nodiscard]] glm::vec3 GetWAnglesRad() const;
    [[nodiscard]] glm::vec3 GetWAnglesDeg() const;
    [[nodiscard]] glm::vec3 GetWScale() const;

    [[nodiscard]] glm::vec3 GetWPos();
    [[nodiscard]] glm::quat GetWRot();
    [[nodiscard]] glm::vec3 GetWAnglesRad();
    [[nodiscard]] glm::vec3 GetWAnglesDeg();
    [[nodiscard]] glm::vec3 GetWScale();

    void SetWPos(glm::vec3 wp);
    void SetWRot(glm::quat wr);
    void SetWAnglesRad(glm::vec3 war);
    void SetWAnglesDeg(glm::vec3 wad);
    void SetWScale(glm::vec3 ws);

    [[nodiscard]] glm::mat4 GetMatrix() const;
    [[nodiscard]] glm::mat4 GetMatrix();
    void SetMatrix(glm::mat4 matrix);

    [[nodiscard]] bool IsDirty() const;
    void SetDirty();
    void RecalculateWorld();

    void AddChangeCallback(std::function<void(const std::shared_ptr<Node>&)> onChange);
    //#endregion

    //#region Children
    [[nodiscard]] const std::unordered_set<std::shared_ptr<Node>>& GetChildren() const;
    [[nodiscard]] size_t GetChildCount() const;

    std::shared_ptr<Node> CreateChild();
    void AttachChild(const std::shared_ptr<Node>& target);

    void DetachChild(const std::shared_ptr<Node>& target);
    void DetachAllChildren();
    //#endregion

    [[nodiscard]] bool IsEnabled() const;
    void SetIsEnabled(bool newIsEnabled);

private:
    explicit Node(std::shared_ptr<Node> parent);

    bool isEnabled = true;

    std::string label;

    std::shared_ptr<Node> parent;
    std::unordered_set<std::shared_ptr<Node>> children;

    glm::vec3 lPos = glm::zero<glm::vec3>();
    glm::quat lRot = glm::zero<glm::vec3>();
    glm::vec3 lScale = glm::zero<glm::vec3>();

    glm::vec3 wPos = glm::zero<glm::vec3>();
    glm::quat wRot = glm::zero<glm::vec3>();
    glm::vec3 wScale = glm::zero<glm::vec3>();

    bool dirty = false;

    std::vector<std::function<void(const std::shared_ptr<Node>&)>> changeCallbacks;
};