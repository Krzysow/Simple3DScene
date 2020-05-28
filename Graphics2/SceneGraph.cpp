#include "SceneGraph.h"

bool SceneGraph::Initialise(void)
{
	for (SceneNodePointer childNode : _children)
	{
		if (!childNode->Initialise())
		{
			return false;
		}
	}
	return true;
}

void SceneGraph::Update(FXMMATRIX& currentWorldTransformation)
{
	SceneNode::Update(currentWorldTransformation);

	for (SceneNodePointer childNode : _children)
	{
		childNode->Update(XMLoadFloat4x4(&_combinedWorldTransformation));
	}
}

void SceneGraph::Render(void)
{
	for (SceneNodePointer childNode : _children)
	{
		childNode->Render();
	}
}

void SceneGraph::Shutdown(void)
{
	for (SceneNodePointer childNode : _children)
	{
		childNode->Shutdown();
	}
}

void SceneGraph::Add(SceneNodePointer node)
{
	_children.push_back(node);
}

void SceneGraph::Remove(SceneNodePointer node)
{
	for (SceneNodePointer childNode : _children)
	{		
		if (childNode == node)
		{
			_children.remove(childNode);
			return;
		}
		else
		{
			childNode->Remove(node);
		}
	}
}

SceneNodePointer SceneGraph::Find(wstring name)
{
	SceneNodePointer response = SceneNode::Find(name);
	if (response != nullptr)
	{
		return response;
	}
	else
	{
		for (SceneNodePointer childNode : _children)
		{
			response = childNode->Find(name);
			if (response != nullptr)
			{
				return response;
			}
		}
	}
	return nullptr;
}
