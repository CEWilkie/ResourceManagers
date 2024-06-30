//
// Created by cew05 on 17/06/2024.
//

#ifndef RESOURCEMANAGERLIB_TEXTUREMANAGER_H
#define RESOURCEMANAGERLIB_TEXTUREMANAGER_H

#include "GenericManager.h"
#include "SDL.h"
#include "SDL_image.h"

struct TextureInfo {
    std::string path;
    [[maybe_unused]] std::pair<int, int> grid {1, 1};
    unsigned int count = 0;
};

class TextureManager {
    public:
        enum class Issue {
            NO_ISSUE, ALREADY_EXISTS, DOES_NOT_EXIST, REQUIRED_PATH_EMPTY, RENDERER_NULL, CANNOT_LOAD
        };

    private:
        std::unordered_map<int, SDL_Texture *>* resourceMap;
        std::unordered_map<int, TextureInfo>* infoMap;

        std::vector<Issue>* issues;

        SDL_Renderer* renderer;

        bool LoadTexture(int _id) {
            if (!TextureExists(_id)) {
                issues->push_back(Issue::DOES_NOT_EXIST);
                return false;
            }

            if ((*infoMap)[_id].path.empty() && (*resourceMap)[_id] == nullptr) {
                issues->push_back(Issue::REQUIRED_PATH_EMPTY);
                return false;
            }

            if ((*resourceMap)[_id] != nullptr) return true;

            (*resourceMap)[_id] = IMG_LoadTexture(renderer, (*infoMap)[_id].path.c_str());
            if ((*resourceMap)[_id] == nullptr) {
                printf("SDL Issue occurred: %s\n", SDL_GetError());
                issues->push_back(Issue::CANNOT_LOAD);
                return false;
            }

            return true;
        }

        bool UnloadTexture(int _id) {
//            if (!TextureExists(_id)) {
//                issues->push_back(Issue::DOES_NOT_EXIST);
//                return false;
//            }
//
//            if ((*infoMap)[_id].path.empty()) {
//                issues->push_back(Issue::REQUIRED_PATH_EMPTY);
//                return false;
//            }
//
//            if ((*resourceMap)[_id] == nullptr) return true;
//
//            SDL_DestroyTexture((*resourceMap)[_id]);
//            (*resourceMap)[_id] = nullptr;
            return true;
        }

    public:
        explicit TextureManager(SDL_Renderer* _renderer) {
            resourceMap = new std::unordered_map<int, SDL_Texture *>;
            infoMap = new std::unordered_map<int, TextureInfo>;
            issues = new std::vector<Issue>;

            renderer = _renderer;
        }

        bool ChangeRenderer(SDL_Renderer* _renderer) {
            if (_renderer == nullptr) {
                issues->push_back(Issue::RENDERER_NULL);
                return false;
            }

            renderer = _renderer;
            return true;
        }

        bool TextureExists(int _id) {
            return resourceMap->count(_id) > 0 && infoMap->count(_id) > 0;
        }

        bool NewTexture(const std::string &_path, int _id) {
            if (TextureExists(_id)) {
                issues->push_back(Issue::ALREADY_EXISTS);
                return false;
            }

            (*resourceMap)[_id] = nullptr;
            (*infoMap)[_id] = {_path};
            return true;
        }

        bool NewTexture(const std::string &_path, std::pair<int, int> _grid, int _id) {
            if (TextureExists(_id)) {
                issues->push_back(Issue::ALREADY_EXISTS);
                return false;
            }

            (*resourceMap)[_id] = nullptr;
            (*infoMap)[_id] = {_path, _grid};
            return true;
        }

        bool NewTexture(SDL_Texture* _texture, int _id) {
            if (TextureExists(_id)) {
                issues->push_back(Issue::ALREADY_EXISTS);
                return false;
            }

            (*resourceMap)[_id] = _texture;
            (*infoMap)[_id] = {};
            return true;
        }

        bool UpdateTexture(SDL_Texture* _texture, int _id) {
            if (!TextureExists(_id)) {
                issues->push_back(Issue::DOES_NOT_EXIST);
                return false;
            }

            (*resourceMap)[_id] = _texture;
            return true;
        }

        bool UpdateInfo(const std::string& _path, int _id) {
            if (!TextureExists(_id)) {
                issues->push_back(Issue::DOES_NOT_EXIST);
                return false;
            }

            (*infoMap)[_id].path = _path;
            return true;
        }

        bool UpdateInfo(const std::string& _path, std::pair<int, int> _grid, int _id) {
            if (!TextureExists(_id)) {
                issues->push_back(Issue::DOES_NOT_EXIST);
                return false;
            }

            (*infoMap)[_id].path = _path;
            (*infoMap)[_id].grid = _grid;
            return true;
        }

        bool UpdateInfo(std::pair<int, int> _grid, int _id) {
            if (!TextureExists(_id)) {
                issues->push_back(Issue::DOES_NOT_EXIST);
                return false;
            }

            (*infoMap)[_id].grid = _grid;
            return true;
        }

        SDL_Texture* AccessTexture(int _id) {
            if (!TextureExists(_id)) {
                issues->push_back(Issue::DOES_NOT_EXIST);
                return nullptr;
            }

            return (*resourceMap)[_id];
        }

        TextureInfo* AccessInfo(int _id) {
            if (!TextureExists(_id)) {
                issues->push_back(Issue::DOES_NOT_EXIST);
                return nullptr;
            }

            return &(*infoMap)[_id];
        }

        bool DestroyTexture(int _id) {
            if (!TextureExists(_id)) {
                issues->push_back(Issue::DOES_NOT_EXIST);
                return false;
            }

            if ((*resourceMap)[_id] != nullptr) SDL_DestroyTexture((*resourceMap)[_id]);
            resourceMap->erase(_id);
            infoMap->erase(_id);
            return true;
        }

        bool OpenTexture(int _id) {
            if (!TextureExists(_id)) {
                issues->push_back(Issue::DOES_NOT_EXIST);
                return false;
            }

            if (!LoadTexture(_id)) return false;
            (*infoMap)[_id].count++;
            return true;
        }

        bool CloseTexture(int _id) {
            if (!TextureExists(_id)) {
                issues->push_back(Issue::DOES_NOT_EXIST);
                return false;
            }

            if (!UnloadTexture(_id)) return false;
            (*infoMap)[_id].count--;
            return true;
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

        static std::string GetIssueString(Issue _issue) {
            std::string issueStr;

            switch (_issue) {
                case Issue::NO_ISSUE:
                    issueStr = "No issues remain";
                    break;
                case Issue::ALREADY_EXISTS:
                    issueStr = "specified id is already in use by another resource";
                    break;
                case Issue::DOES_NOT_EXIST:
                    issueStr = "no resource using specified id exists";
                    break;
                case Issue::REQUIRED_PATH_EMPTY:
                    issueStr = "path required to load the resource is empty";
                    break;
                case Issue::RENDERER_NULL:
                    issueStr = "renderer is null and cannot create textures";
                    break;
                case Issue::CANNOT_LOAD:
                    issueStr = "failed to load the texture. See SDL_GetError()";
                    break;
                default:
                    issueStr = "Unknown IssueID";
                    break;
            }

            return issueStr;
        }
};

#endif //RESOURCEMANAGERLIB_TEXTUREMANAGER_H
