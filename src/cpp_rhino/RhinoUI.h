#pragma once
#include "stdafx.h"


inline int RhinoFindOrCreateLayer(CRhinoDoc& doc, const wchar_t* layer_name) {
	if (nullptr == layer_name || 0 == layer_name[0])
		return ON_UNSET_INT_INDEX;

	int layer_index = doc.m_layer_table.FindLayerFromFullPathName(layer_name, ON_UNSET_INT_INDEX);


	if (layer_index == ON_UNSET_INT_INDEX) {
		ON_Layer layer;
		layer.SetName(layer_name);
		layer.SetColor(ON_Color(100, 100, 100));
		layer.SetPlotColor(ON_Color(100, 100, 100));
		layer.SetPlotWeight(2);
		layer_index = doc.m_layer_table.AddLayer(layer);
	}


	//Set newly created layer as the main one
	int current_layer_index = doc.m_layer_table.CurrentLayerIndex();
	doc.m_layer_table.SetCurrentLayerIndex(layer_index,true);

	//Hide the previous current one
	ON_Layer layer_current = doc.m_layer_table[current_layer_index];
	layer_current.SetVisible(false);
	layer_current.SetPersistentVisibility(false);
	doc.m_layer_table.ModifyLayer(layer_current, current_layer_index, true);



	return layer_index;
}