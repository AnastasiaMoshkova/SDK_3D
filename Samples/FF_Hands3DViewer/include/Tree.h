/*******************************************************************************
 
INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2016 Intel Corporation. All Rights Reserved.
 
*******************************************************************************/
#pragma once

#ifndef TREE_H
#define	TREE_H

#include <vector>

#include "pxcsensemanager.h"


struct PointData 
{
	pxcI32            confidence;          /// RESERVED: for future confidence score feature
	PXCPoint3DF32     positionWorld;       /// The geometric position in 3D world coordinates, in meters
	PXCPoint3DF32     positionImage;       /// The geometric position in 2D image coordinates, in pixels. (Note: the Z coordinate is the point's depth in millimeters.)
	PXCPoint4DF32     localRotation;       /// A quaternion representing the local 3D orientation of the joint, relative to its parent joint
	PXCPoint4DF32     globalOrientation;   /// A quaternion representing the global 3D orientation, relative to the "world" y axis
	PXCPoint3DF32     speed;               /// The speed of the joints in 3D world coordinates (X speed, Y speed, Z speed, in meters/second)
};

template <class T>
class Node
{
public:
	Node(T nodeValue);
	Node();
	void add(Node<T> node);
	T getNodeValue();
	std::vector<Node<T>> getChildNodes();
	~Node();
private:
	T m_nodeValue;
	std::vector<Node<T>> m_nodes;
};

template <class T>
class Tree
{
public:
	Tree(Node<T> rootNode);
	Tree();
	void setRoot(Node<T> rootNode);
	Node<T> getRoot();
	~Tree();

private:
	Node<T> m_root;
};




#endif	/* TREE_H */