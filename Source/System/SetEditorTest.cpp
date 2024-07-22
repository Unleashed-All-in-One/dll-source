#include "SetEditorTest.h"
DWORD* SetManager;
bool initSet;
static const int* pColID_BasicTerrain = reinterpret_cast<int*>(0x01E0AFAC);
struct MultiSetParam
{
	BB_INSERT_PADDING(0x4);
	BYTE m_Field04;
	BYTE m_Field05;
	BYTE m_Field06;
	BYTE m_Field07;
	BYTE m_Field08;
	int m_Field09;
	void* m_Field16;
	BYTE gap8[4];
	hh::map<Hedgehog::Base::CSharedString*, boost::shared_ptr<void*>> map;
	boost::shared_ptr<void*> m_Unknown; //not a shared ptr, struct of some kind
};
BB_ASSERT_OFFSETOF(MultiSetParam, m_Field16, 0x10);
BB_ASSERT_OFFSETOF(MultiSetParam, map, 0x18);
struct SSetObjectCreationInfo
{
	Eigen::Vector3f m_Position;
	float m_UnknownFloat; // Always 2.0????
	Hedgehog::Math::CQuaternion m_Rotation;
	Hedgehog::Base::CSharedString m_Name;
	int m_Field024;
	MultiSetParam* m_MultiSetParam;
	Sonic::CEditParam* m_EditParam;
	float m_Range;
	int m_Field034;
	int m_Field038;
	int m_LayerID;
	int m_Field040;
	void* m_Field044;
	int m_Field048;
	int m_Field04C;
};
inline FUNCTION_PTR(char, __thiscall, SetObjectManager_ProcessMessageF, 0x00EB3BE0, DWORD* This, Hedgehog::Universe::Message&, int flag);
//uint32_t __thiscall sub_EB2A80(DWORD *this, int a2, int a3, int a4)
HOOK(char, __fastcall, SetObjectManager_ProcessMessage, 0x00EB3BE0, DWORD* This, void* Edx, Hedgehog::Universe::Message& message, int flag)
{
	SetManager = This;
	return originalSetObjectManager_ProcessMessage(This, Edx, message, flag);
}
void SetEditorTest::remakeAllObjects()
{	
	Sonic::Message::MsgRemakeAllSetObject msg = Sonic::Message::MsgRemakeAllSetObject();
	SetObjectManager_ProcessMessageF(SetManager, msg, 0);
}
void SetEditorTest::activateLayer(const char* m_SetLayerName)
{
	Sonic::Message::MsgActivateLayer msg = Sonic::Message::MsgActivateLayer("base", true);
	SetObjectManager_ProcessMessageF(SetManager, msg, 0);
}
void SetEditorTest::deactivateLayer(const char* m_SetLayerName)
{
	Sonic::Message::MsgDeactivateLayer msg = Sonic::Message::MsgDeactivateLayer("base");
	SetObjectManager_ProcessMessageF(SetManager, msg, 0);
}
bool showSetTest = false;
std::string nameLayer;
float objectMatrix[1][16] = {
  { 1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f },
};
static const char* getTypeName(const void* object)
{
	try
	{
		class _dummy_virtual_class
		{
			virtual ~_dummy_virtual_class() = 0;
		};
		return typeid(*reinterpret_cast<const _dummy_virtual_class*>(object)).name();
		
	}
	catch(std::exception& e)
	{
		return "invalid";
	}
}

std::vector<Sonic::CGameObject3D*> m_GameObject3Ds;
int selectedIndex;
int objectCategory;
Sonic::CGameObject3D* m_CurrentObjectSelected;
bool m_HasParsedObjectInfo;
bool m_IsSelectedGameObject3D;
static ImGuizmo::OPERATION m_CurrentGizmoOperation(ImGuizmo::ROTATE);
static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
bool m_IsButtonBeingPressed;
float previewCubeTranslation[3];
float previewCubeRotation[3];
float previewCubeScale[3];
void DisplayGizmo(boost::shared_ptr<Sonic::CCamera> camera, float* data)
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	float* transformMatrixData = data;
	if (ImGuizmo::Manipulate(camera->m_MyCamera.m_View.data(), camera->m_MyCamera.m_Projection.data(), m_CurrentGizmoOperation, mCurrentGizmoMode, transformMatrixData, NULL, NULL, NULL))
	{
		m_CurrentObjectSelected->m_spMatrixNodeTransform->m_Transform.m_Matrix = Eigen::Map<const Eigen::Matrix4f>(transformMatrixData);
		m_CurrentObjectSelected->m_spMatrixNodeTransform->NotifyChanged();

	}
}
void LayerWindow()
{
	if (showSetTest)
	{
		auto layerMap = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager->m_mSetLayers;
		ImGui::SetNextWindowSize({ 480, 980 }, ImGuiCond_Always);
		ImGui::SetNextWindowPos({ 20, 2160 - 1000 }, ImGuiCond_Always);
		if (showSetTest && ImGui::Begin("title###Layermenu", &showSetTest, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
		{
			ImGui::Dummy(ImVec2(1, 1));
			ImGui::Indent(1 * ImGui::GetStyle().IndentSpacing / 2);
			ImGui::TextUnformatted("Layer Shortcuts");

			ImGui::Indent(2 * ImGui::GetStyle().IndentSpacing / 2);
			auto firstValue = layerMap.at(0);
			ImGui::TextUnformatted(std::format("Current Layer : {}", firstValue->m_LayerName.c_str()).c_str());
			
			ImGui::TextUnformatted("---------------------------------------------------------");
			
			for (auto it = layerMap.begin(); it != layerMap.end(); ++it)
			{

				char buffer[4];
				sprintf(buffer, "%02d", it->first);
				ImGui::TextUnformatted(buffer);
				ImGui::SameLine(0, 5 * ImGui::GetStyle().ItemSpacing.x);
				ImGui::TextUnformatted("ON");
				ImGui::SameLine(0, 3 * ImGui::GetStyle().ItemSpacing.x);
				ImGui::TextUnformatted(it->second->m_LayerName.c_str());
			}
			ImGui::End();
		}
	}
}
hh::math::CVector4 ScreenToWorld(const hh::math::CVector4& screenPos, const boost::shared_ptr<Sonic::CCamera>& camera) {
	float normalizedX = ((screenPos.x() )  / ImGui::GetIO().DisplaySize.x);
	float normalizedY = (-screenPos.y() / ImGui::GetIO().DisplaySize.y);

	// Homogeneous coordinates
	Eigen::Vector4f normalizedScreenPos(normalizedX, normalizedY, 1.0f, 1.0f);

	// Step 2: Inverse Projection Transformation
	Eigen::Matrix4f inverseProjectionMatrix = camera->m_MyCamera.m_Projection.inverse();
	Eigen::Vector4f viewPos = inverseProjectionMatrix * normalizedScreenPos;

	// Step 3: De-homogenize (reverse the perspective divide)
	viewPos /= viewPos.w();

	// Step 4: Inverse View Transformation
	Eigen::Matrix4f inverseViewMatrix = camera->m_MyCamera.m_View.inverse().matrix();
	Eigen::Vector4f worldDirection = inverseViewMatrix * viewPos;

	// Normalize direction
	worldDirection -= CVector4(camera->m_Position.x(), camera->m_Position.y(), camera->m_Position.z(), 1);
	worldDirection.normalize();

	return worldDirection;
}

std::vector< SSetObjectCreationInfo*> creationInfos;

int numb;
struct MsgStartSetEditor : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0X0167EEDC);
};
struct MsgDebugDrawPath : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0X0167EF04);
};

struct SNode1
{
	SNode1* pLeft;
	SNode1* pParent;
	SNode1* pRight;
};
struct ActivationElement
{
	int field00;
	boost::shared_ptr<void*> field04;
};


class CActivationCullingManager
{
public:
	boost::shared_ptr<void*> m_spRangeCullingMethod;
	//void* m_Unknown1; //
	boost::shared_ptr<void*> m_spFrustumCullingMethod;
	void* m_Unknown2; // These are probably shared ptrs and regular ptr pairs
	boost::shared_ptr<void*> m_spChangedInvokerCullingMethod;
	void* m_Unknown3; //
	//boost::shared_ptr<void*> m_Unknown4;
	boost::shared_ptr<void*> m_spResetFlagCullingMethod;
	void* m_Unknown5;
};

struct Test
{
	void* test00;
	void* test01;
	void* test02;
	void* test03;
	void* test04;
	void* test05;
	void* test06;
	void* test07;
	void* test08;
	void* test09;
	void* test10;
};
struct ActivationListenerForSetObject
{
	DWORD dword0;
	void* dword4;
	float range;
	SSetObjectCreationInfo* creationInfo;
	Hedgehog::math::CVector dword10;
	void* dword14;
	void* dword18;
	void* dword1C;
};
BB_ASSERT_OFFSETOF(ActivationListenerForSetObject, creationInfo, 0xC);
class CActivation
{
public:
	boost::shared_ptr <void*>gap;
	BYTE byte8;
	BYTE byte9;
	BYTE byteA;
	BYTE byteB;
	void* dwordC;
	void* dword10;
	void* dword14;
	CVector4 m_Position;
	CVector4 m_Rotation;
	boost::shared_ptr<ActivationListenerForSetObject> m_spListenerForSetObject;
	boost::shared_ptr<Sonic::CActivationManager> m_spActivationManager;
	boost::shared_ptr<void*> m_spCullingElementID;
	int m_spListenerForSetObjectFunc3;
	float m_spListenerForSetObjectFunc4;
	void* dword5C;
};
BB_ASSERT_OFFSETOF(CActivation, dword10, 0x10);
BB_ASSERT_OFFSETOF(CActivation, m_spListenerForSetObject, 0x40);
struct SElementSub
{
	boost::shared_ptr<CActivation> test;
	int one, two;
};
struct SElement
{
	void* field00;
	hh::vector<SElementSub> m_ActivationList;
	void* field12;
	void* field16;
	void* field20;
};
BB_ASSERT_OFFSETOF(SElement, m_ActivationList, 4);
//BB_ASSERT_OFFSETOF(SElement, field12, 12);
class ActivationManager : public Sonic::CGameObject
{
public:
	void* m_Unknown2;
	hh::map<void*, boost::shared_ptr<SElement>> m_ElementMap;
	CActivationCullingManager* m_pActivationCullingManager;
	boost::shared_ptr<CActivationCullingManager> m_spActivationCullingManager;
	BB_INSERT_PADDING(36-4);
	hh::list<Test> possiblyAMap;
	//Hedgehog::Universe::Message* msgGetCullingBasePosition; //???
	//BB_INSERT_PADDING(0x8);
	//void* m_Unknown7;
	//void* m_Unknown8;
	//BYTE m_Unknown9;
	//void* m_Unknown10;
};
BB_ASSERT_OFFSETOF(ActivationManager, m_ElementMap, 0xB0-4);
//BB_ASSERT_OFFSETOF(ActivationManager, possiblyAMap, 0xE8 - 4);
//BB_ASSERT_OFFSETOF(ActivationManager, possiblyAMap, 0xE8-4);
class ParameterBankC2
{
public:
	Hedgehog::base::CSharedString test1[0xC];
	hh::map<Sonic::CParamBase*, int > m_ParamList;
};
struct __declspec(align(2)) struct_result
{
	DWORD* dword0;
	DWORD* dword4;
	DWORD* dword8;
	BYTE gapC[8];
	BYTE byte14;
	BYTE byte15;
};
class testgap
{
public:
	Hedgehog::base::CSharedString* field00;
	Hedgehog::base::CSharedString* field01;
	Hedgehog::base::CSharedString* field02;
	Hedgehog::base::CSharedString* field03;
	int field04;
	int field05;
	int field06;
	int field07;
	int field08;
	int field09;
	int field10;
};
struct CParameterBank : public Hedgehog::Base::CRefCountObject
{
public:
	hh::vector<Sonic::CParamValue<int>> children;
	int32_t field18;
	std::map<void*, void*> field1C;
	int32_t field24;
	int32_t field28;
	std::map<void*, void*> field2C;
	int32_t field34;
};
//BB_ASSERT_OFFSETOF(CParameterBank, children, 0x8);
//BB_ASSERT_OFFSETOF(CParameterBank, children, 0x10);
class ParameterBankC
{
public:
	BB_INSERT_PADDING(0xC);
	Hedgehog::base::CSharedString field;
	hh::list<void*> m_ParamList;
};
class ParameterBankManager
{
public:
	hh::map<int, CParameterBank*> dword4;
	hh::map<int, boost::shared_ptr<void*>> dword8;
	void* dword14;
};
class CParamFloat: public Sonic::CParamBase
{
public:
	class FuncData : public Hedgehog::Base::CRefCountObject
	{
	public:
		float* m_pValue;
		float m_Value;
		boost::function1<void, float> m_ChangedCallback;
		Hedgehog::Base::CSharedString m_Description;
	};

	FuncData* m_pFuncData;
	float m_DefaultValue;
};
Sonic::CEditParam* testParam;

void GameObjectList()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
	const boost::shared_ptr<Sonic::CCamera> camera = Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera();
	if (ImGui::Begin("GameObject List"))
	{
		if (ImGui::CollapsingHeader("GameObjects", true))
		{
			ImGui::Text("Index: %d", selectedIndex);
			int index = 0;
			for (boost::shared_ptr<Sonic::CGameObject> data : Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects)
			{
				if (dynamic_cast<Sonic::CSetObjectListener*>(data.get()) != nullptr)
					m_GameObject3Ds.push_back(dynamic_cast<Sonic::CGameObject3D*>(data.get()));

				index++;
				ImGui::PushID(index + 100);
				if (ImGui::Selectable(getTypeName(data.get()), false, ImGuiSelectableFlags_None))
				{
					selectedIndex = index;
					m_HasParsedObjectInfo = false;
					m_IsSelectedGameObject3D = false;
					objectCategory = 0;
				}
				ImGui::PopID();
			}
		}
		if (ImGui::CollapsingHeader("GameObjects (3D)", true))
		{
			for (int i = 0; i < m_GameObject3Ds.size(); ++i)
			{
				ImGui::PushID(i + 200);
				if (ImGui::Selectable(getTypeName(m_GameObject3Ds[i]), false, ImGuiSelectableFlags_None))
				{
					selectedIndex = i;
					m_HasParsedObjectInfo = false;
					m_IsSelectedGameObject3D = true;
					objectCategory = 1;
				}
				if (ImGui::IsItemHovered())
				{
					DisplayGizmo(camera, m_GameObject3Ds[i]->m_spMatrixNodeTransform->m_Transform.m_Matrix.data());
				}
				ImGui::PopID();
			}
		}
		ImGui::End();
	}
	if (ImGui::Begin("Inspector"))
	{

		ImGui::Text("Index: %d", selectedIndex);

		if (Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.size() > selectedIndex || m_GameObject3Ds.size() > selectedIndex)
		{
			if (!m_HasParsedObjectInfo)
			{
				m_HasParsedObjectInfo = true;
				if (objectCategory == 0) //non 3d objects
				{
					hh::list<boost::shared_ptr<Sonic::CGameObject>>::iterator iterator = Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.begin();
					std::advance(iterator, selectedIndex);
					if (dynamic_cast<Sonic::CGameObject3D*>(iterator->get()) != nullptr)
					{
						//init tool preview
						m_CurrentObjectSelected = dynamic_cast<Sonic::CGameObject3D*>(iterator->get());
						m_IsSelectedGameObject3D = true;
					}
					else
						m_IsSelectedGameObject3D = false;
				}
				else //3d objects
				{
					m_IsSelectedGameObject3D = true;
					m_CurrentObjectSelected = m_GameObject3Ds[selectedIndex];
					auto activationmanager = (ActivationManager*)Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pActivationManager;
					auto map = activationmanager->m_ElementMap;
					FUNCTION_PTR(bool, __thiscall, SetObjectSendMessage, 0x00EB3A60,
						Hedgehog::Universe::CMessageActor * _messageActor,
						Sonic::CSetObjectManager * _setObjectManager,
						uint32_t setObjectID,
						boost::shared_ptr<Hedgehog::Universe::Message>);

					bool skip = false;

					for (size_t i = 0; i < creationInfos.size(); i++)
					{
						//auto preview = std::string(creationInfos[i]->m_Name.c_str());
						//if (std::string(getTypeName(m_CurrentObjectSelected)).find(preview) == std::string::npos)
						//	continue;
						float distance = abs(ObjectUtility::Distance(creationInfos[i]->m_Position, m_GameObject3Ds[selectedIndex]->m_spMatrixNodeTransform->m_Transform.m_Position));
						if (distance <= 5)
						{
							testParam = creationInfos[i]->m_EditParam;
							break;
						}
					}

				}

			}
			if (m_HasParsedObjectInfo && m_IsSelectedGameObject3D)
			{
				if (ImGui::CollapsingHeader("Transform"))
				{


					if (GetAsyncKeyState(VK_MBUTTON) < 0 && m_IsButtonBeingPressed == false)
					{
						m_IsButtonBeingPressed = true;
						if (m_CurrentGizmoOperation == ImGuizmo::TRANSLATE)
							m_CurrentGizmoOperation = ImGuizmo::ROTATE;
						else
							m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
					}
					if (GetAsyncKeyState(VK_MBUTTON) == 0 && m_IsButtonBeingPressed == true)
					{
						m_IsButtonBeingPressed = false;
					}
					if (GetAsyncKeyState(VK_LBUTTON) < 0 && m_IsButtonBeingPressed == false)
					{
						m_IsButtonBeingPressed = true;
						auto e = ScreenToWorld(CVector4(ImGui::GetMousePos().x, ImGui::GetMousePos().y, 0, 1), camera);
						CVector4 fOut;
						CVector4 fOut2;
						const CVector4 startPos = CVector4(camera->m_Position.x() + e.x(), camera->m_Position.y() + e.y(), camera->m_Position.z() + e.z(), 1);
						//e = e.normalized();
						CVector rightVector = camera->m_MyCamera.m_View.matrix().block<3, 1>(0, 0).normalized();
						const CVector4 endPos = CVector4(e.x(), e.y(), e.z(), 1) * 1000;
						Eigen::Vector4f rayDirection = CVector4(camera->m_MyCamera.m_Direction.normalized().x(), camera->m_MyCamera.m_Direction.normalized().y(), camera->m_MyCamera.m_Direction.normalized().z(), 1);

						// Construct the ray end position
						//Eigen::Vector4f rayEndPos = (e * rayDirection) * 1000.0f;
						//if(Common::fRaycast(startPos, startPos + rayDirection * 1000.0f, fOut, fOut2, *pColID_BasicTerrain))
						//{
						//	m_CurrentObjectSelected->m_spMatrixNodeTransform->m_Transform.SetPosition(CVector(fOut.x(), fOut.y(), fOut.z()));
						//	m_CurrentObjectSelected->m_spMatrixNodeTransform->NotifyChanged();
						//	DebugDrawText::log(std::format("HIT: {0}, {1}, {2}", fOut.x(), fOut.y(), fOut.z()).c_str(), 0);
						//	DebugDrawText::log(std::format("SCREEN TO WORLD POS: {0}, {1}, {2}", e.x(), e.y(), e.z()).c_str(), 0);
						//	DebugDrawText::log(std::format("END RAY: {0}, {1}, {2}", endPos.x(), endPos.y(), endPos.z()).c_str(), 0);
						//}
					}
					if (GetAsyncKeyState(VK_LBUTTON) == 0 && m_IsButtonBeingPressed == true)
					{
						m_IsButtonBeingPressed = false;
					}
					//auto& position1 = m_CurrentObjectSelected->m_spMatrixNodeTransform->m_Transform.m_Position;
					//hh::math::CVector4 screenPosition = camera->m_MyCamera.m_View * hh::math::CVector4(position1.x(), position1.y(), position1.z(), 1.0f);
					//screenPosition = camera->m_MyCamera.m_Projection * screenPosition;
					//screenPosition.head<2>() /= screenPosition.w();
					//screenPosition.x() = ((screenPosition.x() * 0.5f) * (LetterboxHelper::OriginalResolution->x()));
					//screenPosition.y() = (screenPosition.y() * -0.5f) * (LetterboxHelper::OriginalResolution->y());
					//DebugDrawText::draw("Test", DebugDrawText::Location(screenPosition.x(), screenPosition.y()), 3);
					//if (GetAsyncKeyState(VK_LBUTTON) < 0 && m_IsButtonBeingPressed == false)
					//{
					//	m_IsButtonBeingPressed = true;
					//	CVector2 screen_pos = CVector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
					//	float distanceBestCanditate = 1000;
					//	int closestIndex = -1;
					//	Sonic::CGameObject3D* target = nullptr;
					//	for (size_t i = 0; i < vec1.size(); i++)
					//	{
					//		if (dynamic_cast<Sonic::CGameObject3D*>(vec1[i].get()) != nullptr)
					//		{
					//			//init tool preview
					//			target = dynamic_cast<Sonic::CGameObject3D*>(vec1[i].get());
					//			auto& position2 = target->m_spMatrixNodeTransform->m_Transform.m_Position;
					//			const auto camera = Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera();
					//			hh::math::CVector4 screenPosition2 = camera->m_MyCamera.m_View * hh::math::CVector4(position2.x(), position2.y(), position2.z(), 1.0f);
					//			screenPosition2 = camera->m_MyCamera.m_Projection * screenPosition2;
					//			screenPosition2.head<2>() /= screenPosition2.w();
					//			screenPosition2.x() = ((screenPosition2.x() * 0.5f + 0.5f) * (LetterboxHelper::OriginalResolution->x()));
					//			screenPosition2.y() = (screenPosition2.y() * -0.5f + 0.5f) * (LetterboxHelper::OriginalResolution->y());
					//			CVector2 diff = CVector2(screenPosition2.x(), screenPosition2.y()) - screen_pos;
					//			float distance = abs(sqrtf(diff.dot(diff)));
					//			if (distance < distanceBestCanditate)
					//			{
					//				distanceBestCanditate = distance;
					//				closestIndex = i;
					//			}
					//		}
					//		
					//	}
					//	DebugDrawText::log(std::format("DISTANCE CANDIDATE: {0}\nINDEX: {1}", distanceBestCanditate, closestIndex).c_str());
					//	selectedIndex = closestIndex;
					//	
					//
					//}
					//if (GetAsyncKeyState(VK_LBUTTON) == 0 && m_IsButtonBeingPressed == true)
					//{
					//	m_IsButtonBeingPressed = false;
					//}
					float vectorPosition[3] = { m_CurrentObjectSelected->m_spMatrixNodeTransform->m_Transform.m_Position.x(), m_CurrentObjectSelected->m_spMatrixNodeTransform->m_Transform.m_Position.y(), m_CurrentObjectSelected->m_spMatrixNodeTransform->m_Transform.m_Position.z() };
					float vectorRotation[3] = { m_CurrentObjectSelected->m_spMatrixNodeTransform->m_Transform.m_Rotation.x(), m_CurrentObjectSelected->m_spMatrixNodeTransform->m_Transform.m_Position.y(), m_CurrentObjectSelected->m_spMatrixNodeTransform->m_Transform.m_Position.z() };
					DisplayGizmo(camera, m_CurrentObjectSelected->m_spMatrixNodeTransform->m_Transform.m_Matrix.data());


					if (ImGui::InputFloat3("Position", vectorPosition))
					{
						m_CurrentObjectSelected->m_spMatrixNodeTransform->m_Transform.SetPosition(CVector(vectorPosition[0], vectorPosition[1], vectorPosition[2]));
						m_CurrentObjectSelected->m_spMatrixNodeTransform->NotifyChanged();
					}
					if (ImGui::InputFloat3("Rotation", vectorRotation))
					{
						m_CurrentObjectSelected->m_spMatrixNodeTransform->m_Transform.m_Rotation.x() = vectorRotation[0];
						m_CurrentObjectSelected->m_spMatrixNodeTransform->m_Transform.m_Rotation.y() = vectorRotation[1];
						m_CurrentObjectSelected->m_spMatrixNodeTransform->m_Transform.m_Rotation.z() = vectorRotation[2];
						m_CurrentObjectSelected->m_spMatrixNodeTransform->NotifyChanged();
					}
				}
				if (ImGui::CollapsingHeader("SetParameter"))
				{
					for (size_t i = 0; i < testParam->m_ParamList.size(); i++)
					{
						if (dynamic_cast<Sonic::CParamBool*>(testParam->m_ParamList[i]) != nullptr)
						{
							Sonic::CParamBool* boole = ((Sonic::CParamBool*)testParam->m_ParamList[i]);
							//if (boole->m_pFuncData == nullptr)
							//	continue;
							ImGui::Checkbox(testParam->m_ParamList[i]->m_Name.c_str(), &boole->m_DefaultValue);
						}
						if (dynamic_cast<CParamFloat*>(testParam->m_ParamList[i]) != nullptr)
						{
							CParamFloat* boole = ((CParamFloat*)testParam->m_ParamList[i]);
							//if (boole->m_pFuncData == nullptr)
							//	continue;
							ImGui::InputFloat(testParam->m_ParamList[i]->m_Name.c_str(), &boole->m_DefaultValue);
						}
						//if (dynamic_cast<Sonic::CParamValue<int>>(testParam->m_ParamList[i]) != nullptr)
						//{
						//	Sonic::CParamBool* boole = ((Sonic::CParamBool*)testParam->m_ParamList[i]);
						//	//if (boole->m_pFuncData == nullptr)
						//	//	continue;
						//	ImGui::Checkbox(testParam->m_ParamList[i]->m_Name.c_str(), &boole->m_DefaultValue);
						//}

						//if (dynamic_cast<Sonic::CParamValue<int>*>(testParam->m_ParamList[i]) != nullptr)
						//	ImGui::InputInt(testParam->m_ParamList[i]->m_Name.c_str(), ((Sonic::CParamValue<int>*)testParam->m_ParamList[i])->m_pFuncData->m_pValue);
						//if (dynamic_cast<Sonic::CParamValue<float>*>(testParam->m_ParamList[i]) != nullptr)
						//	ImGui::InputFloat(testParam->m_ParamList[i]->m_Name.c_str(), ((Sonic::CParamValue<float>*)testParam->m_ParamList[i])->m_pFuncData->m_pValue);
					}
				}
			}
		}
		ImGui::End();
	}
}boost::shared_ptr<Hedgehog::Mirage::CMaterialData> SetEditorTest::materialData;

void SetEditorTest::draw()
{
	if (!initSet)
		return;
	ImGui::SetNextWindowBgAlpha(1);

	//LayerWindow();
	//GameObjectList();
	if (ImGui::Begin("Material"))
	{
		
		if(materialData != nullptr)
		{
			for (size_t i = 0; i < materialData->m_Float4Params.size(); i++)
			{
				ImGui::InputFloat4(materialData->m_Float4Params[i]->m_Name.GetValue(), materialData->m_Float4Params[i]->m_spValue.get());				
			}
		}
		ImGui::End();
	}
	
	if (ImGui::Begin("Test"))
	{
		for (size_t i = 0; i < creationInfos.size(); i++)
		{
			//for (size_t x = 0; x < creationInfos.at(i)->m_EditParam->m_ParamList.size(); x++)
			//{
			//	ImGui::Text(creationInfos.at(i)->m_EditParam->m_ParamList.at(x)->m_Name.c_str());
			//}
			ImGui::Separator();
		}
		ImGui::End();
	}
	m_GameObject3Ds.clear();
}
#define  _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING
bool init = false;
HOOK(void*, __fastcall, SetUpdateApplication, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	if (GetAsyncKeyState(VK_F10) && !init)
	{
		init = true;
		//auto ptr = (uint32_t*)(Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember);
		//auto e = *(void**)((ptr + 16)+4);
		//int size = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetObjectFactory->m_MakeList.size();
		//auto size2 = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_spSetObjectManager->m_pMember->test;
		////std::vector<int> t2 = std::vector(Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->list.begin(), Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->list.end());
		//auto sub = (uint32_t*)Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_spSetObjectManager->m_pMember;
		//auto test1 = (uint32_t*)(sub + 6);
		//auto test2 = *(DWORD*)(test1 + 4);
		//auto test3 = *(DWORD*)(test2 + 4);
		////xstd::vector<uint32_t> vec1 = std::vector(size2->begin(), size2->end());
		////for (size_t i = 0; i < vec1.size(); i++)
		////{
		////	auto fff = vec1.at(i);
		////}
		////for (size_t i = 0; i < size; i++)
		////{
		////	
		////	DebugDrawText::log(Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetObjectFactory->m_MakeList[i].m_Name.c_str());
		////	Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetObjectFactory->m_MakeList[i].m_pMake->pFunc()->m_spSetObjectInfo->spSetObjectListener;
		////	printf("");
		////}
		
		std::vector<boost::shared_ptr<Sonic::CGameObject>> vec1 = std::vector(Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.begin(), Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.end());
		auto member = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember;
		Sonic::CSetLayerManager* t = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager.get();
		ParameterBankManager* paramBank = (ParameterBankManager*)Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spParameterBankManager.get();
		auto aaf = t->m_mSetLayers.size();
		/*for (auto it = member->m_Field1C->list.begin(); it != member->m_Field1C->list.end(); ++it)
		{
			auto first = it->first;
			auto second = it->second;

			auto testString = (Hedgehog::base::CSharedString*)second;
		}*/
		auto appdoc = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spSequenceMain;
		auto activationmanager = (ActivationManager*)Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pActivationManager;
		auto map = activationmanager->m_ElementMap;

		for (auto it = map.begin(); it != map.end(); ++it)
		{
			auto one = it->second;
			for (size_t i = 0; i < one->m_ActivationList.size(); i++)
			{
				auto two = one->m_ActivationList[i].test;
			}
		}

		for (auto it = t->m_mSetLayers.begin(); it != t->m_mSetLayers.end(); ++it)
		{
			auto one = it->first;
			auto two = it->second;
			auto three = it->second.get();//
			DebugDrawText::log(std::format("LAYER {0}\nName: {1}\nFilename: {2}\nSaving Location: {3}\ndwordC: {4}",one, three->m_LayerName.c_str(), three->m_FileName.c_str(), three->m_SaveLocation.c_str(), three->dwordC.x()).c_str());
		}
		for (auto it = paramBank->dword4.begin(); it != paramBank->dword4.end(); ++it)
		{
			auto one = it->first;
			auto two =it->second;
			//auto three = two->dword10;
			//auto four = three.at(0)
			//auto four = three.begin();
			DebugDrawText::log(std::format("PARAMBANK: {0}", one).c_str());
		}
		//for (std::set<boost::shared_ptr<void*>>::iterator it = t->dword4.begin(); it != t->dword4.end(); it++) {
		//	//^^                   //^^
		//	SetLayer* fa = (SetLayer*)it->get();
		//	printf("");
		//}
		for (size_t i = 0; i < Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.size(); i++)
		{
			void* vptrA = nullptr;
			try
			{
				vptrA = *((void**)vec1.at(i).get());
			}
			catch(std::exception& e)
			{
				continue;
			}
			vec1.at(i)->SendMessageImm(vec1.at(i)->m_ActorID, boost::make_shared<MsgDebugDrawPath>());

			if (dynamic_cast<Sonic::CSetObjectListener*>(vec1.at(i).get()) != nullptr)
			{
				//init tool preview
				dynamic_cast<Sonic::CSetObjectListener*>(vec1.at(i).get())->OnSetEditorEnter();
			}
		}
		//auto t = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager->test;
		/*int size2 = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager->test->vector.size();
		auto ssize2 = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager->test;
		std::vector<Hedgehog::base::CSharedString> test = std::vector(Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager->test->vector.begin(), Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager->test->vector.end());
		for (size_t i = 0; i < size2; i++)
		{
			
			DebugDrawText::log(test.at(i).c_str());
			printf("");
		}*/
		//auto t = ->Test;
		MsgStartSetEditor msg = MsgStartSetEditor();
		SetObjectManager_ProcessMessageF(SetManager, msg, 0);
	}
	if (GetAsyncKeyState(VK_F11) && init)
	{
		init = false;

		std::vector<boost::shared_ptr<Sonic::CGameObject>> vec1 = std::vector(Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.begin(), Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.end());

		for (size_t i = 0; i < Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.size(); i++)
		{
			void* vptrA = nullptr;
			try
			{
				vptrA = *((void**)vec1.at(i).get());
			}
			catch (std::exception& e)
			{
				continue;
			}
			vec1.at(i)->SendMessageImm(vec1.at(i)->m_ActorID, boost::make_shared<MsgDebugDrawPath>());

			if (dynamic_cast<Sonic::CSetObjectListener*>(vec1.at(i).get()) != nullptr)
			{
				//kill tool preview
				dynamic_cast<Sonic::CSetObjectListener*>(vec1.at(i).get())->OnSetEditorLeave();
			}
		}
		
		MsgStartSetEditor msg = MsgStartSetEditor();
		SetObjectManager_ProcessMessageF(SetManager, msg, 0);
	}
	return originalSetUpdateApplication(This, Edx, elapsedTime, a3);
}

//int *__stdcall sub_EA67D0(int a1, int layerIndex, char in_Toggle)
HOOK(int*, __stdcall, sub_EA67D0, 0xEA67D0, void* a1, int layerIndex, char in_Toggle)
{
	void* vftable = *(void**)a1;
	return originalsub_EA67D0(a1, layerIndex, in_Toggle);
}
//void *__thiscall Sonic::CInstanceBrush::~CInstanceBrush(void *this, char a2)
HOOK(void*, __fastcall, CInstanceBrushDestructor, 0x00598BD0, void* This, void* Edx, char a2)
{
	showSetTest = false;
	return originalCInstanceBrushDestructor(This, Edx, a2);
}
//void __thiscall Sonic::CGameplayFlowStageAct::StartInstanceBrush(int *this, int a2)
HOOK(void, __fastcall, StartInstanceBrush, 0x00D07610, void* This, void* Edx, int a2)
{
	showSetTest = true;
	originalStartInstanceBrush(This, Edx, a2);
}
//char __thiscall sub_597E50(_DWORD *this, Hedgehog::Math::CVector *a2, Hedgehog::Math::CVector *a3, __m128 *a4, __m128 *a5)
HOOK(char, __fastcall, sub_597E50, 0x597E50, DWORD* This, void* Edx, Hedgehog::Math::CVector* a2, Hedgehog::Math::CVector* a3, Hedgehog::Math::CVector* a4, __m128* a5)
{
	Sonic::Player::CPlayerSpeedContext::GetInstance()->m_spMatrixNode->m_Transform.SetPosition(*a4 );
	auto four = a4;
	return originalsub_597E50(This, Edx, a2,a3,a4,a5);
}


struct Activation
{
	void* dword4;
	void* dword8;
	boost::shared_ptr<SSetObjectCreationInfo> dwordC;
	SSetObjectCreationInfo* dword10;
	void* dword14;
	void* dword18;
	void* dword1C;
	
};
//int *__stdcall CreateCActivationListenerForSetObject(int a1, int a2, _DWORD *creationInfo, int a4, int a5)
HOOK(int*, __stdcall, CreateCActivationListenerForSetObject, 0xEA6BD0, int a1, int a2, boost::shared_ptr<SSetObjectCreationInfo>* creationInfo, int a4, int a5)
{
	auto e = creationInfo->get();
	creationInfos.push_back(e);
	return originalCreateCActivationListenerForSetObject(a1, a2, creationInfo, a4, a5);
}
//BOOL __stdcall sub_CEFCD0(int a1)
HOOK(bool, __stdcall, sub_CEFCD0, 0xCEFCD0, int a1)
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	auto keyState = (Sonic::EKeyState)a1;
	if(keyState == Sonic::eKeyState_RightBumper)
	{
		if (GetAsyncKeyState(VK_SPACE))
			return true;
	}
	if (keyState == Sonic::eKeyState_LeftBumper)
	{
		if (GetAsyncKeyState(VK_CONTROL))
			return true;
	}
	if (keyState == Sonic::eKeyState_X)
	{
		if (GetAsyncKeyState(VK_SHIFT))
			return true;
	}
	if (GetAsyncKeyState('W'))
		inputPtr->LeftStickVertical = 1;
	if (GetAsyncKeyState('S'))
		inputPtr->LeftStickVertical = -1;
	if (GetAsyncKeyState('A'))
		inputPtr->LeftStickHorizontal = -1;
	if (GetAsyncKeyState('D'))
		inputPtr->LeftStickHorizontal = 1;

	if (GetAsyncKeyState(VK_UP))
		inputPtr->RightStickVertical = 1;
	if (GetAsyncKeyState(VK_DOWN))
		inputPtr->RightStickVertical = -1;
	if (GetAsyncKeyState(VK_RIGHT))
		inputPtr->RightStickHorizontal = 1;
	if (GetAsyncKeyState(VK_LEFT))
		inputPtr->RightStickHorizontal = -1;
	return originalsub_CEFCD0(a1);
}
void SetEditorTest::applyPatches()
{
	WRITE_JUMP(0x00FD6AB3, 0x00FD6ABF);
	//WRITE_JUMP(0x00EB681D, ASMTEST);
	//INSTALL_HOOK(ADD_SET_OBJECTS_TO_WORLD);
	INSTALL_HOOK(CreateCActivationListenerForSetObject);
	INSTALL_HOOK(sub_CEFCD0);
	INSTALL_HOOK(sub_597E50);
	INSTALL_HOOK(StartInstanceBrush);
	INSTALL_HOOK(CInstanceBrushDestructor);
	INSTALL_HOOK(sub_EA67D0);
	INSTALL_HOOK(SetUpdateApplication);
	INSTALL_HOOK(SetObjectManager_ProcessMessage);
	initSet = true;
}