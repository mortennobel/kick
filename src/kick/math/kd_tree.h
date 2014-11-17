#pragma once

#include "kick/math/aabb.h"
#include "kick/math/ray.h"
#include "glm/glm.hpp"
#include <functional>
#include <vector>

namespace kick {




    struct KDTreeHitInfo;

    struct KDTreeNodeRef {
        KDTreeNodeRef();
        KDTreeNodeRef(const KDTreeNodeRef& other);

        glm::vec3 triangle[3];
        int id;

        bool intersectRay(const Ray &ray, KDTreeHitInfo& hit) const;
        AABB getAABB() const;
    };
    struct KDTreeHitInfo {
        float distance = std::numeric_limits<float>::max();
        KDTreeNodeRef const *hit;
        glm::vec3 collisionPoint;
    };

    class KDTreeNode {
    public:
        KDTreeNode *leftChild;
        KDTreeNode *rightChild;
        AABB aabb;
        float splittingPlane;
        int splittingAxis;
        std::vector<KDTreeNodeRef> objects;

        KDTreeNode(const AABB &aabb);



        bool isLeaf();
    };

// Abstract KDTree
    class KDTree {
    public:
        /*
        *  treeDepth: The max depth of tree nodes
        *  minVoxelObjects: The minimum number of objects in a tree node (if less, node is not subdivided)
        */
        KDTree();
        ~KDTree(void);
        virtual void init(std::vector<KDTreeNodeRef> &objectList, AABB &aabb);

        virtual bool first_intersection(Ray &ray, KDTreeHitInfo& hit);

    protected:
        std::vector<KDTreeNodeRef> objectList;
        KDTreeNode *rootNode;
        AABB aabb;
        bool intersect(KDTreeNode *child, const Ray &ray, KDTreeHitInfo& hit, glm::vec3 startPoint);
        KDTreeNode *recursiveBuild(const std::vector<KDTreeNodeRef>& objectList, AABB &aabb, int currentDepth);
        virtual bool terminateBuild(const std::vector<KDTreeNodeRef>& objectList, AABB &aabb, int currentDepth) = 0;
        /***
         * Find optimal splitting plane
         */
        virtual void findPlane(const std::vector<KDTreeNodeRef>& objectList, AABB &aabb, int currentDepth, KDTreeNode *node) = 0;
};


    // The Simple KD tree uses a näive spatial median as splitting plane
class KDTreeNaive : public KDTree {
public:
    /*
*  treeDepth: The max depth of tree nodes
*  minVoxelObjects: The minimum number of objects in a tree node (if less, node is not subdivided)
*/
	KDTreeNaive(int treeDepth = 12, int minVoxelObjects = 20);
protected:

    bool terminateBuild(const std::vector<KDTreeNodeRef>& objectList, AABB &aabb, int currentDepth);

    /***
	 * Find optimal splitting plane
	 */
	void findPlane(const std::vector<KDTreeNodeRef>& objectList, AABB &aabb, int currentDepth, KDTreeNode *node);
	
private:
	int treeDepth;
    int minVoxelObjects;
};

}
	