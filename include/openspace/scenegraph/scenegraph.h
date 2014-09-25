/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014                                                                    *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  *
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  *
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                         *
 ****************************************************************************************/
 
#ifndef __SCENEGRAPH_H__
#define __SCENEGRAPH_H__

// std includes
#include <vector>
#include <map>

#include <openspace/util/camera.h>

#include <openspace/scripting/scriptengine.h>

// ghoul includes
#include <ghoul/opengl/programobject.h>
#include <ghoul/misc/dictionary.h>
#include <openspace/util/runtimedata.h>

namespace openspace {


class SceneGraphNode;

class SceneGraph {
public:
    // constructors & destructor
    SceneGraph();
    ~SceneGraph();

	void setRuntimeData(RuntimeData* runtimeData);

    /**
     * Initalizes the SceneGraph by loading modules from the ${SCENEPATH} directory
     */
    bool initialize();

    /*
     * Clean up everything
     */
    bool deinitialize();

    /*
     * Load the scenegraph from the provided folder
     */
    bool loadScene(const std::string& sceneDescriptionFilePath);

    void loadModule(const std::string& modulePath);

    /*
     * Updates all SceneGraphNodes relative positions
     */
    void update();

    /*
     * Evaluates if the SceneGraphNodes are visible to the provided camera
     */
    void evaluate(Camera* camera);

    /*
     * Render visible SceneGraphNodes using the provided camera
     */
    void render(Camera* camera);

    /*
     * Prints the SceneGraph tree. For debugging purposes
     */
    void printChildren() const;

    /*
     * Returns the root SceneGraphNode
     */
    SceneGraphNode* root() const;

    /**
     * Return the scenegraph node with the specified name or <code>nullptr</code> if that
     * name does not exist
     */
    SceneGraphNode* sceneGraphNode(const std::string& name) const;

	/**
	 * Returns the Lua library that contains all Lua functions available to change the
	 * scene graph. The functions contained are
	 * - openspace::luascriptfunctions::property_setValue
	 * - openspace::luascriptfunctions::property_getValue
	 * \return The Lua library that contains all Lua functions available to change the
	 * scene graph
	 */
	static scripting::ScriptEngine::LuaLibrary luaLibrary();

private:
    std::string _focus, _position;

    // actual scenegraph
    SceneGraphNode* _root;
    std::vector<SceneGraphNode*> _nodes;
    std::map<std::string, SceneGraphNode*> _allNodes;
	RuntimeData* _runtimeData;
};

} // namespace openspace

#endif // __SCENEGRAPH_H__
