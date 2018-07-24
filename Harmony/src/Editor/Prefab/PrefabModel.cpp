/**************************************************************************
 *	PrefabModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "PrefabModel.h"
#include "Project.h"
#include "AtlasWidget.h"

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/Exporter.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags

QString StdVectorIntsToQString(const std::vector<int> &vectorRef)
{
	if(vectorRef.empty())
		return "<empty>";

	QString sRetValue = QString::number(vectorRef[0]);
	for(uint i = 1; i < static_cast<uint>(vectorRef.size()); ++i)
		sRetValue += ", " % QString::number(vectorRef[i]);
	
	return sRetValue;
}

PrefabModel::PrefabModel(ProjectItem &itemRef, QJsonValue initValue) :	IModel(itemRef),
																		m_PropertiesModel(itemRef, 0, 0, this)
{
	if(initValue.isString() && initValue.toString().contains(HYGUI_ImportPrefix))	// Importing a new prefab asset
	{
		QString sInitValue = initValue.toString().remove(HYGUI_ImportPrefix);

		// Import the DCC file from artist
		Assimp::Importer importer;
		const aiScene *pScene = importer.ReadFile(sInitValue.toStdString(), aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
		if(pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
		{
			HyGuiLog(sInitValue + " was incomplete and is invalid", LOGTYPE_Error);
			return;
		}

		// Ensure the destination directory exists (<DataDir>/Prefabs/<prefix>/)
		QDir dataDir(itemRef.GetProject().GetAssetsAbsPath() % HyGlobal::ItemName(ITEM_Prefab, true));
		if(false == dataDir.mkpath(itemRef.GetPrefix()))
		{
			HyGuiLog("PrefabModel::PrefabModel() - Creating directory path failed: " % itemRef.GetPrefix(), LOGTYPE_Error);
			return;
		}
		QDir prefabDir(dataDir.absolutePath() % "/" % itemRef.GetPrefix());
		QString sAbsFilePath = prefabDir.absoluteFilePath(itemRef.GetName(false) % ".gltf");

		// Using the imported DCC file, export it to a glTF 2.0 at the above destination
		Assimp::Exporter exporter;
		const aiExportFormatDesc *pDesc = nullptr;
		for(uint i = 0; i < exporter.GetExportFormatCount(); ++i)
		{
			pDesc = exporter.GetExportFormatDescription(i);
			if(strcmp(pDesc->description, "GL Transmission Format v. 2") == 0)
				break;
		}
		aiReturn ret = exporter.Export(pScene, pDesc->id, sAbsFilePath.toStdString());
		if(ret != aiReturn_SUCCESS)
		{
			HyGuiLog(sAbsFilePath + " failed to export to glTF", LOGTYPE_Error);
			return;
		}

		// Open and parse the newly exported glTF file. Any image dependencies should be imported into the project's atlas manager.
		// Modify the glTF file to also include the "checksum" of these imported images, so they can be queried by Harmony Engine.
		QFile gltfFile(sAbsFilePath);
		if(gltfFile.exists())
		{
			if(!gltfFile.open(QIODevice::ReadOnly))
				HyGuiLog(QString("PrefabModel::PrefabModel() could not open ") % sAbsFilePath, LOGTYPE_Error);

			QJsonDocument gltfDoc = QJsonDocument::fromJson(gltfFile.readAll());
			QJsonObject gltfObj = gltfDoc.object();
			gltfFile.close();

			// Find all the required image dependencies and store their file paths in 'sImageFilePaths'
			QStringList sImageFilePaths;
			QList<quint32> imageChecksumList;
			QJsonArray imagesArray = gltfObj["images"].toArray();
			for(int i = 0; i < imagesArray.size(); ++i)
			{
				QJsonObject imageObj = imagesArray[i].toObject();
				if(imageObj.find("uri") != imageObj.end())
				{
					QFileInfo importFileInfo(sInitValue);
					sImageFilePaths.append(importFileInfo.absolutePath() % "/" % imageObj["uri"].toString());
				}
				else if(imageObj.find("bufferView") != imageObj.end())
					HyGuiLog("TODO: glTF files which store their images in the accompanied binary blob file are not yet supported. They should either use a \"uri\" reference or once implemented, be stripped out after importing.", LOGTYPE_Info);
				else
					HyGuiLog("PrefabModel::PrefabModel() - while importing, glTF images did not have either a \"uri\" or a \"bufferView\": " % sAbsFilePath, LOGTYPE_Error);
			}

			// Import images into the selected atlas group, or default one
			QList<AtlasFrame *> newlyImportedAtlasFrameList;
			if(sImageFilePaths.size() == imagesArray.size())
			{
				quint32 uiAtlasGrpId = itemRef.GetProject().GetAtlasModel().GetAtlasGrpIdFromAtlasGrpIndex(0);
				if(itemRef.GetProject().GetAtlasWidget())
					uiAtlasGrpId = itemRef.GetProject().GetAtlasWidget()->GetSelectedAtlasGrpId();

				for(int i = 0; i < sImageFilePaths.size(); ++i)
				{
					QFileInfo fileInfo(sImageFilePaths[i]);
					QImage newImage(fileInfo.absoluteFilePath());
					newlyImportedAtlasFrameList.append(itemRef.GetProject().GetAtlasModel().ImportImage(fileInfo.baseName(), newImage, uiAtlasGrpId, ITEM_Prefab, nullptr));
				}
				
				// Repack the affected atlas group to generate the final textures and finish setting the AtlasFrames' data
				QSet<AtlasFrame *> newlyImportedAtlasFrameSet = newlyImportedAtlasFrameList.toSet();
				if(newlyImportedAtlasFrameSet.empty() == false)
					itemRef.GetProject().GetAtlasModel().Repack(itemRef.GetProject().GetAtlasModel().GetAtlasGrpIndexFromAtlasGrpId(uiAtlasGrpId), QSet<int>(), newlyImportedAtlasFrameSet);

				// Associate the newly added frames with this prefab item
				itemRef.GetProject().GetAtlasModel().RequestFrames(&itemRef, newlyImportedAtlasFrameList);
			}
			else
				HyGuiLog("PrefabModel::PrefabModel() - sImageFilePaths.size() != imagesArray.size(): " % sAbsFilePath, LOGTYPE_Error);

			// Insert the checksum values into the images array
			for(int i = 0; i < imagesArray.size(); ++i)
			{
				QJsonObject imageObj = imagesArray[i].toObject();
				imageObj.insert("checksum", QJsonValue(static_cast<qint64>(newlyImportedAtlasFrameList[i]->GetImageChecksum())));
				imagesArray[i] = imageObj;
			}
			gltfObj.insert("images", imagesArray);

			// Resave glTF file with inserted checksums on images
			if(gltfFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
				HyGuiLog("Couldn't open " % sAbsFilePath % " for writing: " % gltfFile.errorString(), LOGTYPE_Error);
			else
			{
				QJsonDocument userDoc;
				userDoc.setObject(gltfObj);
				qint64 iBytesWritten = gltfFile.write(userDoc.toJson());
				if(0 == iBytesWritten || -1 == iBytesWritten)
					HyGuiLog("Could not write to " % sAbsFilePath % ": " % gltfFile.errorString(), LOGTYPE_Error);

				gltfFile.close();
			}
		}
		else
			HyGuiLog("PrefabModel::PrefabModel() could not find the exported gltf file: " % sAbsFilePath, LOGTYPE_Error);
	}

	QDir dataDir(itemRef.GetProject().GetAssetsAbsPath() % HyGlobal::ItemName(ITEM_Prefab, true));
	QDir prefabDir(dataDir.absolutePath() % "/" % itemRef.GetPrefix());
	QString sAbsFilePath = prefabDir.absoluteFilePath(itemRef.GetName(false) % ".gltf");

	tinygltf::TinyGLTF loader;
	std::string sError;
	bool bLoadSuccess = loader.LoadASCIIFromFile(&m_ModelData, &sError, sAbsFilePath.toStdString(), tinygltf::REQUIRE_ALL);
	if(bLoadSuccess == false)
		HyLogError("Loading glTF file failed: " << sError.c_str());

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// SCENES
	for(uint i = 0; i < static_cast<uint>(m_ModelData.scenes.size()); ++i)
	{
		QString sCategoryName = "Scene " % QString::number(i);
		m_PropertiesModel.AppendCategory(sCategoryName, QColor(255, 224, 192), QVariant(), false, false, "The prefab may contain scenes (with an optional default scene). Each scene can contain an array of indices of nodes.");
		
		m_PropertiesModel.AppendProperty(sCategoryName, "Default Scene", PropertiesDef(PROPERTIESTYPE_bool, i == m_ModelData.defaultScene), "", true);
		m_PropertiesModel.AppendProperty(sCategoryName, "Name", PropertiesDef(PROPERTIESTYPE_LineEdit, QString(m_ModelData.scenes[i].name.c_str())), "", true);
		m_PropertiesModel.AppendProperty(sCategoryName, "Node Indices", PropertiesDef(PROPERTIESTYPE_LineEdit, StdVectorIntsToQString(m_ModelData.scenes[i].nodes)), "", true);
		for(auto iter = m_ModelData.scenes[i].extensions.begin(); iter != m_ModelData.scenes[i].extensions.end(); ++iter)
		{
			QString sPropertyName = "Ext: " % QString(iter->first.c_str());
			AppendGltfValueProperty(sCategoryName, sPropertyName, iter->second, "");
		}
		AppendGltfValueProperty(sCategoryName, "Extras", m_ModelData.scenes[i].extras, "");
	}

	// NODES
	for(uint i = 0; i < static_cast<uint>(m_ModelData.nodes.size()); ++i)
	{
		QString sCategoryName = "Node " % QString::number(i);
		m_PropertiesModel.AppendCategory(sCategoryName, QColor(255, 224, 192), QVariant(), false, false, "The prefab may contain scenes (with an optional default scene). Each scene can contain an array of indices of nodes.");

		m_PropertiesModel.AppendProperty(sCategoryName, "Name", PropertiesDef(PROPERTIESTYPE_LineEdit, QString(m_ModelData.nodes[i].name.c_str())), "", true);
		if(m_ModelData.nodes[i].mesh >= 0)
			m_PropertiesModel.AppendProperty(sCategoryName, "Mesh Index", PropertiesDef(PROPERTIESTYPE_int, m_ModelData.nodes[i].mesh), "", true);
		if(m_ModelData.nodes[i].skin >= 0)
			m_PropertiesModel.AppendProperty(sCategoryName, "Skin Index", PropertiesDef(PROPERTIESTYPE_int, m_ModelData.nodes[i].skin), "", true);
		if(m_ModelData.nodes[i].camera >= 0)
			m_PropertiesModel.AppendProperty(sCategoryName, "Camera Index", PropertiesDef(PROPERTIESTYPE_int, m_ModelData.nodes[i].camera), "", true);
		if(m_ModelData.nodes[i].children.empty() == false)
		{
			if(m_ModelData.nodes[i].children.size() == 1)
				m_PropertiesModel.AppendProperty(sCategoryName, "Child Index", PropertiesDef(PROPERTIESTYPE_int, m_ModelData.nodes[i].children[0]), "", true);
			else
				m_PropertiesModel.AppendProperty(sCategoryName, "Children Indices", PropertiesDef(PROPERTIESTYPE_LineEdit, StdVectorIntsToQString(m_ModelData.nodes[i].children)), "", true);
		}
		if(m_ModelData.nodes[i].matrix.size() == 16)
		{
			glm::mat4 mtx(m_ModelData.nodes[i].matrix[0], m_ModelData.nodes[i].matrix[1], m_ModelData.nodes[i].matrix[2], m_ModelData.nodes[i].matrix[3],
						  m_ModelData.nodes[i].matrix[4], m_ModelData.nodes[i].matrix[5], m_ModelData.nodes[i].matrix[6], m_ModelData.nodes[i].matrix[7],
						  m_ModelData.nodes[i].matrix[8], m_ModelData.nodes[i].matrix[9], m_ModelData.nodes[i].matrix[10], m_ModelData.nodes[i].matrix[11],
						  m_ModelData.nodes[i].matrix[12], m_ModelData.nodes[i].matrix[13], m_ModelData.nodes[i].matrix[14], m_ModelData.nodes[i].matrix[15]);
			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 translation;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(mtx, scale, rotation, translation, skew, perspective);

			m_PropertiesModel.AppendProperty(sCategoryName, "Translation", PropertiesDef(PROPERTIESTYPE_vec3, QRectF(translation[0], translation[1], translation[2], 0)), "", true);
			m_PropertiesModel.AppendProperty(sCategoryName, "Rotation", PropertiesDef(PROPERTIESTYPE_vec4, QRectF(rotation[0], rotation[1], rotation[2], rotation[3])), "", true);
			m_PropertiesModel.AppendProperty(sCategoryName, "Scale", PropertiesDef(PROPERTIESTYPE_vec3, QRectF(scale[0], scale[1], scale[2], 0)), "", true);
		}
		else
		{
			if(m_ModelData.nodes[i].translation.empty() == false)
				m_PropertiesModel.AppendProperty(sCategoryName, "Translation", PropertiesDef(PROPERTIESTYPE_vec3, QRectF(m_ModelData.nodes[i].translation[0], m_ModelData.nodes[i].translation[1], m_ModelData.nodes[i].translation[2], 0)), "", true);
			if(m_ModelData.nodes[i].rotation.empty() == false)
				m_PropertiesModel.AppendProperty(sCategoryName, "Rotation", PropertiesDef(PROPERTIESTYPE_vec4, QRectF(m_ModelData.nodes[i].rotation[0], m_ModelData.nodes[i].rotation[1], m_ModelData.nodes[i].rotation[2], m_ModelData.nodes[i].rotation[3])), "", true);
			if(m_ModelData.nodes[i].scale.empty() == false)
				m_PropertiesModel.AppendProperty(sCategoryName, "Scale", PropertiesDef(PROPERTIESTYPE_vec3, QRectF(m_ModelData.nodes[i].scale[0], m_ModelData.nodes[i].scale[1], m_ModelData.nodes[i].scale[2], 0)), "", true);
		}
		for(auto iter = m_ModelData.nodes[i].extensions.begin(); iter != m_ModelData.nodes[i].extensions.end(); ++iter)
		{
			QString sPropertyName = "Ext: " % QString(iter->first.c_str());
			AppendGltfValueProperty(sCategoryName, sPropertyName, iter->second, "");
		}
		AppendGltfValueProperty(sCategoryName, "Extras", m_ModelData.nodes[i].extras, "");
	}

	// MESHES
	for(uint i = 0; i < static_cast<uint>(m_ModelData.meshes.size()); ++i)
	{
		QString sCategoryName = "Mesh " % QString::number(i);
		m_PropertiesModel.AppendCategory(sCategoryName, QColor(224, 255, 192), QVariant(), false, false, "May contain multiple mesh primitives. These refer to the geometry data that is required for rendering the mesh.");

		m_PropertiesModel.AppendProperty(sCategoryName, "Name", PropertiesDef(PROPERTIESTYPE_LineEdit, QString(m_ModelData.meshes[i].name.c_str())), "", true);
		for(auto iter = m_ModelData.meshes[i].extensions.begin(); iter != m_ModelData.meshes[i].extensions.end(); ++iter)
		{
			QString sPropertyName = "Ext: " % QString(iter->first.c_str());
			AppendGltfValueProperty(sCategoryName, sPropertyName, iter->second, "");
		}
		AppendGltfValueProperty(sCategoryName, "Extras", m_ModelData.meshes[i].extras, "");
	}

	m_PropertiesModel.AppendCategory("Buffers", QColor(192, 255, 192), QVariant(), false, false, "The buffers contain the data that is used for the geometry of 3D models, animations, and skinning.");
	m_PropertiesModel.AppendCategory("Materials", QColor(192, 255, 224), QVariant(), false, false, "Each mesh primitive may refer to one of the materials that are contained in a prefab asset. The materials describe how an object should be rendered.");
	m_PropertiesModel.AppendCategory("Textures", QColor(192, 255, 255), QVariant(), false, false, "Contain information about textures that may be applied to rendered objects.");
}

PropertiesTreeModel &PrefabModel::GetPropertiesModel()
{
	return m_PropertiesModel;
}

//void PrefabModel::CopyNodesWithMeshes( aiNode node, SceneObject targetParent, Matrix4x4 accTransform)
//{
//	SceneObject parent;
//	Matrix4x4 transform;

//	// if node has meshes, create a new scene object for it
//	if(node.mNumMeshes > 0)
//	{
//		SceneObjekt newObject = new SceneObject;
//		targetParent.addChild( newObject);
//		// copy the meshes
//		CopyMeshes( node, newObject);
//		// the new object is the parent for all child nodes
//		parent = newObject;
//		transform.SetUnity();
//	}
//	else
//	{
//		// if no meshes, skip the node, but keep its transformation
//		parent = targetParent;
//		transform = node.mTransformation * accTransform;
//	}

//	// continue for all child nodes
//	for( all node.mChildren)
//	CopyNodesWithMeshes( node.mChildren[a], parent, transform);
//}

/*virtual*/ void PrefabModel::OnSave() /*override*/
{
}

/*virtual*/ QJsonObject PrefabModel::PopStateAt(uint32 uiIndex) /*override*/
{
	return QJsonObject();
}

/*virtual*/ QJsonValue PrefabModel::GetJson() const /*override*/
{
	return QJsonValue(GetItem().GetName(true));
}

/*virtual*/ QList<AtlasFrame *> PrefabModel::GetAtlasFrames() const /*override*/
{
	return QList<AtlasFrame *>();
}

/*virtual*/ QStringList PrefabModel::GetFontUrls() const /*override*/
{
	return QStringList();
}

/*virtual*/ void PrefabModel::Refresh() /*override*/
{
}

void PrefabModel::AppendGltfValueProperty(QString sCategoryName, QString sPropertyName, const tinygltf::Value &valueRef, QString sToolTip)
{
	switch(valueRef.Type())
	{
	case tinygltf::NULL_TYPE:	/*m_PropertiesModel.AppendProperty(sCategoryName, sPropertyName, PropertiesDef(PROPERTIESTYPE_LineEdit, "NULL TYPE"), sToolTip, true);*/ break;
	case tinygltf::NUMBER_TYPE:	m_PropertiesModel.AppendProperty(sCategoryName, sPropertyName, PropertiesDef(PROPERTIESTYPE_double, valueRef.Get<double>()), sToolTip, true); break;
	case tinygltf::INT_TYPE:	m_PropertiesModel.AppendProperty(sCategoryName, sPropertyName, PropertiesDef(PROPERTIESTYPE_int, valueRef.Get<int>()), sToolTip, true); break;
	case tinygltf::BOOL_TYPE:	m_PropertiesModel.AppendProperty(sCategoryName, sPropertyName, PropertiesDef(PROPERTIESTYPE_bool, valueRef.Get<bool>()), sToolTip, true); break;
	case tinygltf::STRING_TYPE:	m_PropertiesModel.AppendProperty(sCategoryName, sPropertyName, PropertiesDef(PROPERTIESTYPE_LineEdit, QString(valueRef.Get<std::string>().c_str())), sToolTip, true); break;
	case tinygltf::ARRAY_TYPE:	m_PropertiesModel.AppendProperty(sCategoryName, sPropertyName, PropertiesDef(PROPERTIESTYPE_LineEdit, "<array type>"), sToolTip, true); break;
	case tinygltf::BINARY_TYPE:	m_PropertiesModel.AppendProperty(sCategoryName, sPropertyName, PropertiesDef(PROPERTIESTYPE_LineEdit, "<binary type>"), sToolTip, true); break;
	case tinygltf::OBJECT_TYPE:	m_PropertiesModel.AppendProperty(sCategoryName, sPropertyName, PropertiesDef(PROPERTIESTYPE_LineEdit, "<object type>"), sToolTip, true); break;
	}
}
