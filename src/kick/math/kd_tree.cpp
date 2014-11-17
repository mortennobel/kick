#include "kick/math/kd_tree.h"

#include <algorithm>
#include <cmath>

using namespace std;
using namespace glm;

namespace kick {
    KDTreeNodeRef::KDTreeNodeRef()
    :id(-1) {

    }

    KDTreeNodeRef::KDTreeNodeRef(const KDTreeNodeRef &other)
            :id(other.id){
        for (int i=0;i<3;i++){
            triangle[i] = other.triangle[i];
        }
    }

    AABB KDTreeNodeRef::getAABB() const {
        AABB res;
        for (int i=0;i<3;i++){
            res.addPoint(triangle[i]);
        }
        return res;
    }

    bool KDTreeNodeRef::intersectRay(const Ray &ray, KDTreeHitInfo &hit) const {
        vec3 intersectionPoint;
        bool res = ray.intersect(triangle[0], triangle[1], triangle[2], intersectionPoint);
        float len = length(intersectionPoint - ray.origin());
        if (res && len < hit.distance){
            hit.distance = len;
            hit.collisionPoint = intersectionPoint;
            hit.hit = this;
        }
        return res;
    }


// ___________________________________________________

    KDTreeNode::KDTreeNode(const AABB &aabb)
            : leftChild(NULL), rightChild(NULL), aabb(aabb) {

    }

    bool KDTreeNode::isLeaf() {
        return leftChild == NULL && rightChild == NULL;
    }

// ___________________________________________________


    KDTree::KDTree()
            : rootNode(NULL) {
    }


    KDTree::~KDTree(void) {
    }

    void KDTree::init(std::vector<KDTreeNodeRef> &objectList, AABB &aabb) {
        this->objectList = objectList;
        this->aabb = aabb;
        this->rootNode = recursiveBuild(objectList, aabb, 0);
    }

    bool KDTree::first_intersection(Ray &ray, KDTreeHitInfo& hit) {
        float tNear, tFar;
        int aNear, aFar;
        bool intersected = aabb.intersect(ray, tNear, tFar,aNear, aFar);
        if (!intersected) {
            return false;
        }

        return intersect(this->rootNode, ray, hit, ray.origin());

    }

// returns true if it has collisions inside aabb
    bool KDTree::intersect(KDTreeNode *child, const Ray &ray, KDTreeHitInfo& hit, vec3 startPoint) {
        bool collision = false;
        if (child->isLeaf()) {
            for (auto& node : child->objects) {
                if (node.intersectRay(ray, hit)){
                    vec3 collisionPos = hit.collisionPoint;
                    collision = child->aabb.contains(collisionPos);
                }
            }
        } else {
            float directionDelta = ray.direction()[child->splittingAxis];

            if (directionDelta < 0 && ray.origin()[child->splittingAxis] < child->splittingPlane) {
                // only consider left child
                collision = intersect(child->leftChild,  ray, hit, startPoint);
            } else if (directionDelta > 0 && ray.origin()[child->splittingAxis] > child->splittingPlane) {
                // only consider right child
                collision = intersect(child->rightChild, ray, hit, startPoint);
            } else {
                // collision with splitting plane
                if (directionDelta > 0) {
                    // if direction is from min to max (left to right)
                    collision = intersect(child->leftChild, ray, hit, startPoint);
                    if (!collision) {
                        float fNear, fFar;
                        int aNear, aFar;
                        if (child->rightChild->aabb.intersect(ray, fNear, fFar,aNear, aFar)) {
                            vec3 splitPlaneIntersect = ray.origin() + ray.direction() * fNear;
                            Ray newRay{splitPlaneIntersect, ray.direction()};
                            collision = intersect(child->rightChild, newRay, hit, startPoint);
                        }
                    }
                } else {
                    // if direction is from min to max (left to right)
                    collision = intersect(child->rightChild, ray, hit, startPoint);
                    if (!collision) {
                        float fNear, fFar;
                        int aNear, aFar;
                        if (child->leftChild->aabb.intersect(ray, fNear, fFar,aNear, aFar)) {
                            vec3 splitPlaneIntersect = ray.origin() + ray.direction() * fNear;
                            Ray newRay{splitPlaneIntersect, ray.direction()};
                            collision = intersect(child->leftChild, newRay, hit, startPoint);
                        }
                    }
                }
            }
        }

        return collision;
    }

    KDTreeNode *KDTree::recursiveBuild(const std::vector<KDTreeNodeRef>& objectList, AABB &aabb, int currentDepth) {
        KDTreeNode *res = new KDTreeNode(aabb);
        if (terminateBuild(objectList, aabb, currentDepth)) {
            for (const KDTreeNodeRef & nodeRef : objectList) {
                res->objects.push_back(nodeRef);
            }
            return res;
        }
        findPlane(objectList, aabb, currentDepth, res);

        // split aabb with splitting plane
        AABB leftAabb(aabb);
        leftAabb.max[res->splittingAxis] = res->splittingPlane;
        AABB rightAabb(aabb);
        rightAabb.min[res->splittingAxis] = res->splittingPlane;

        vector < KDTreeNodeRef > leftObjects;
        vector < KDTreeNodeRef > rightObjects;
        for (const KDTreeNodeRef & nodeRef : objectList) {
            const AABB objectAABB = nodeRef.getAABB();
            if (leftAabb.intersect(objectAABB)) {
                leftObjects.push_back(nodeRef);
            }
            if (rightAabb.intersect(objectAABB)) {
                rightObjects.push_back(nodeRef);
            }
        }
        currentDepth++;
        res->leftChild = recursiveBuild(leftObjects, leftAabb, currentDepth);
        res->rightChild = recursiveBuild(rightObjects, rightAabb, currentDepth);
        return res;
    }

    KDTreeNaive::KDTreeNaive(int treeDepth, int minVoxelObjects)
            : KDTree(), treeDepth(treeDepth), minVoxelObjects(minVoxelObjects) {
    }

    void KDTreeNaive::findPlane(const std::vector<KDTreeNodeRef>& objectList, AABB &aabb, int currentDepth, KDTreeNode *node) {
        int axis = 0;
        vec3 len = aabb.max - aabb.min;
        float maxLen = 0;
        for (int i = 0; i < 3; i++) {
            if (len[i] > maxLen) {
                maxLen = len[i];
                axis = i;
            }
        }
        node->splittingAxis = axis;
        node->splittingPlane = 0.5f * (aabb.min[axis] + aabb.max[axis]);
    }

    bool KDTreeNaive::terminateBuild(const std::vector<KDTreeNodeRef>& objectList, AABB &aabb, int currentDepth) {
        return currentDepth == this->treeDepth || objectList.size() < minVoxelObjects;
    }
}