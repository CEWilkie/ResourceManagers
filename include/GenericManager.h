#ifndef UNTITLED8_LIBRARY_H
#define UNTITLED8_LIBRARY_H

#include <iostream>
#include <vector>
#include <unordered_map>

template<class Resource>
class GenericManager {
    private:
        std::vector<int>* usedKeys = new std::vector<int>;

    protected:
        std::unordered_map<int, Resource>* resourceMap;

        enum class Issue {
            NO_ISSUE, ALREADY_EXISTS, DOES_NOT_EXIST,
        };
        std::vector<Issue>* issues;

    public:
        GenericManager() {
            // Construct resourceMap
            resourceMap = new std::unordered_map<int, Resource>;
        }

        bool ResourceExists(int _id) {
            return resourceMap->count(_id) > 0;
        }

        bool NewResource(Resource _resource, int _id) {
            if (ResourceExists(_id)) {
                issues->push_back(Issue::ALREADY_EXISTS);
                return false;
            }

            (*resourceMap)[_id] = _resource;
            usedKeys->push_back(_id);
            return true;
        }

        void NewOrUpdateResource(Resource _resource, int _id) {
            (*resourceMap)[_id] = _resource;
            usedKeys->push_back(_id);
        }

        bool ChangeResource(Resource _resource, int _id) {
            if (!ResourceExists(_id)) {
                issues->push_back(Issue::DOES_NOT_EXIST);
                return false;
            }

            (*resourceMap)[_id] = _resource;
            return true;
        }

        bool FetchResource(Resource& _resource, int _id) {
            if (!ResourceExists(_id)) {
                issues->push_back(Issue::DOES_NOT_EXIST);
                return false;
            }

            _resource = (*resourceMap)[_id];
            return true;
        }

        std::vector<int> FetchUsedIDs() {
            return *usedKeys;
        }

        std::unordered_map<int, Resource>* AccessMap() {
            return resourceMap;
        }

        /*
         * Issue handling
         */

        [[nodiscard]] Issue GetIssue() {
            if (issues->empty()) return Issue::NO_ISSUE;

            Issue lastIssue = issues->back();
            issues->pop_back();
            return lastIssue;
        }

        void ClearIssues() {
            issues->clear();
        }

        [[nodiscard]] static std::string GetIssueString(Issue _issue) {
            std::string issueStr;

            switch (_issue) {
                case Issue::NO_ISSUE:
                    issueStr = "No issues remain";
                case Issue::ALREADY_EXISTS:
                    issueStr = "specified id is already in use by another resource";
                case Issue::DOES_NOT_EXIST:
                    issueStr = "no resource using specified id exists";
                default:
                    issueStr = "Unknown IssueID";
            }

            return issueStr;
        }
};


#endif //UNTITLED8_LIBRARY_H
