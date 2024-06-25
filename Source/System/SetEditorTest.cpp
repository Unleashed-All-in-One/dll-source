DWORD* SetManager;

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
std::vector<boost::shared_ptr<Sonic::CGameObject>> vec1;
std::vector<Sonic::CGameObject3D*> vec2;
int selectedIndex;
int objectCategory;
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
Sonic::CGameObject3D* go3D;
bool parsedGOInfo;
bool inherits3D;
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
bool shift;
float previewCubeTranslation[3];
float previewCubeRotation[3];
float previewCubeScale[3];
void DisplayGizmo(boost::shared_ptr<Sonic::CCamera> camera, float* data)
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	float* transformMatrixData = data;
	if (ImGuizmo::Manipulate(camera->m_MyCamera.m_View.data(), camera->m_MyCamera.m_Projection.data(), mCurrentGizmoOperation, mCurrentGizmoMode, transformMatrixData, NULL, NULL, NULL))
	{
		go3D->m_spMatrixNodeTransform->m_Transform.m_Matrix = Eigen::Map<const Eigen::Matrix4f>(transformMatrixData);
		go3D->m_spMatrixNodeTransform->NotifyChanged();

	}
}

void SetEditorTest::draw()
{
	ImGui::SetNextWindowBgAlpha(1);
	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
	const auto camera = Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera();

	if(showSetTest)
	{
		auto layerMap = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager->m_mSetLayers;
		
		ImGui::SetNextWindowSize({ 480, 980 }, ImGuiCond_Always);
		ImGui::SetNextWindowPos({ 20, 2160 - 1000 }, ImGuiCond_Always);
		if (showSetTest && ImGui::Begin("title###Layermenu", &showSetTest, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
		{
			/// @separator

			// TODO: Add Draw calls of dependent popup windows here

			/// @begin Text
			ImGui::Dummy(ImVec2(1, 1));
			ImGui::Indent(1 * ImGui::GetStyle().IndentSpacing / 2);
			ImGui::TextUnformatted("Layer Shortcuts");
			/// @end Text

			/// @begin Text
			ImGui::Indent(2 * ImGui::GetStyle().IndentSpacing / 2);
			auto firstValue = layerMap.at(0);
			ImGui::TextUnformatted(std::format("Current Layer : {}", firstValue->m_LayerName.c_str()).c_str());
			/// @end Text

			/// @begin Text
			ImGui::TextUnformatted("---------------------------------------------------------");
			/// @end Text
			
			for (auto it = layerMap.begin(); it != layerMap.end(); ++it)
			{

				char buffer[4];
				sprintf(buffer, "%02d", it->first);
				/// @begin Text
				ImGui::TextUnformatted(buffer);
				/// @end Text

				/// @begin Text
				ImGui::SameLine(0, 5 * ImGui::GetStyle().ItemSpacing.x);
				ImGui::TextUnformatted("ON");
				/// @end Text

				/// @begin Text
				ImGui::SameLine(0, 3 * ImGui::GetStyle().ItemSpacing.x);
				ImGui::TextUnformatted(it->second->m_LayerName.c_str());
				/// @end Text
			}

			/// @separator
			ImGui::End();
		}
		
	}
	vec1 = std::vector(Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.begin(), Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.end());
	for (int i = 0; i < vec1.size(); ++i)
	{
		if (dynamic_cast<Sonic::CGameObject3D*>(vec1[i].get()) != nullptr)
			vec2.push_back(dynamic_cast<Sonic::CGameObject3D*>(vec1[i].get()));
	}
	if (ImGui::Begin("GameObject List"))
	{

		ImGui::Text("Index: %d", selectedIndex);
		if (ImGui::CollapsingHeader("GameObjects (3D)"))
		{
			for (int i = 0; i < vec1.size(); ++i)
			{				
				ImGui::PushID(i + 100);
				if (ImGui::Selectable(getTypeName(vec1[i].get()), false, ImGuiSelectableFlags_None))
				{
					selectedIndex = i;
					parsedGOInfo = false;
					inherits3D = false;
					objectCategory = 0;
				}
				ImGui::PopID();				
			}
		}
		if (ImGui::CollapsingHeader("GameObjects (3D)"))
		{
			for (int i = 0; i < vec2.size(); ++i)
			{

				ImGui::PushID(i + 200);
				if (ImGui::Selectable(getTypeName(vec2[i]), false, ImGuiSelectableFlags_None))
				{
					selectedIndex = i;
					parsedGOInfo = false;
					inherits3D = true;
					objectCategory = 1;
				}
				if (ImGui::IsItemHovered())
				{
					previewCubeTranslation[0] = vec2[i]->m_spMatrixNodeTransform->m_Transform.m_Position.x();	
					previewCubeTranslation[1] = vec2[i]->m_spMatrixNodeTransform->m_Transform.m_Position.y();	
					previewCubeTranslation[2] = vec2[i]->m_spMatrixNodeTransform->m_Transform.m_Position.z();

					previewCubeRotation[0] = vec2[i]->m_spMatrixNodeTransform->m_Transform.m_Rotation.x();
					previewCubeRotation[1] = vec2[i]->m_spMatrixNodeTransform->m_Transform.m_Rotation.y();
					previewCubeRotation[2] = vec2[i]->m_spMatrixNodeTransform->m_Transform.m_Rotation.z();

					previewCubeScale[0] = 1;
					previewCubeScale[1] = 1;
					previewCubeScale[2] = 1;

					float matrix = 0;
					//ImGuizmo::RecomposeMatrixFromComponents(previewCubeTranslation, previewCubeRotation, previewCubeScale,&matrix);

					//ImGuizmo::DrawCubes(camera->m_MyCamera.m_View.data(), camera->m_MyCamera.m_Projection.data(), &matrix, 1);
					DisplayGizmo(camera, vec2[i]->m_spMatrixNodeTransform->m_Transform.m_Matrix.data());
				}
				ImGui::PopID();
			}
		}
	

		/// @separator
		ImGui::End();
	}
	if (ImGui::Begin("Inspector"))
	{

		ImGui::Text("Index: %d", selectedIndex);

		if(vec1.size() > selectedIndex)
		{
			if(!parsedGOInfo)
			{
				parsedGOInfo = true;
				if (objectCategory == 0)
				{
					if (dynamic_cast<Sonic::CGameObject3D*>(vec1[selectedIndex].get()) != nullptr)
					{
						//init tool preview
						go3D = dynamic_cast<Sonic::CGameObject3D*>(vec1[selectedIndex].get());
						inherits3D = true;
					}
					else
						inherits3D = false;
				}
				else
				{
					inherits3D = true;
					go3D = vec2[selectedIndex];
					
				}
				
			}
			if(parsedGOInfo && inherits3D)
			{
				if (ImGui::CollapsingHeader("Transform"))
				{
					auto& position1 = go3D->m_spMatrixNodeTransform->m_Transform.m_Position;
					hh::math::CVector4 screenPosition = camera->m_MyCamera.m_View * hh::math::CVector4(position1.x(), position1.y(), position1.z(), 1.0f);
					screenPosition = camera->m_MyCamera.m_Projection * screenPosition;
					screenPosition.head<2>() /= screenPosition.w();
					screenPosition.x() = ((screenPosition.x() * 0.5f) * (LetterboxHelper::OriginalResolution->x()));
					screenPosition.y() = (screenPosition.y() * -0.5f) * (LetterboxHelper::OriginalResolution->y());
					if (GetAsyncKeyState(VK_MBUTTON) < 0 && shift == false)
					{
						shift = true;
						if (mCurrentGizmoOperation == ImGuizmo::TRANSLATE)
							mCurrentGizmoOperation = ImGuizmo::ROTATE;
						else
							mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
					}
					if (GetAsyncKeyState(VK_MBUTTON) == 0 && shift == true)
					{
						shift = false;
					}
					//if (GetAsyncKeyState(VK_LBUTTON) < 0 && shift == false)
					//{
					//	shift = true;
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
					//if (GetAsyncKeyState(VK_LBUTTON) == 0 && shift == true)
					//{
					//	shift = false;
					//}
					DebugDrawText::draw("Test", DebugDrawText::Location(screenPosition.x(), screenPosition.y()), 7);
					bool result = false;
					float vectorPosition[3] = { go3D->m_spMatrixNodeTransform->m_Transform.m_Position.x(), go3D->m_spMatrixNodeTransform->m_Transform.m_Position.y(), go3D->m_spMatrixNodeTransform->m_Transform.m_Position.z() };
					float vectorRotation[3] = { go3D->m_spMatrixNodeTransform->m_Transform.m_Rotation.x(), go3D->m_spMatrixNodeTransform->m_Transform.m_Position.y(), go3D->m_spMatrixNodeTransform->m_Transform.m_Position.z() };
					DisplayGizmo(camera, go3D->m_spMatrixNodeTransform->m_Transform.m_Matrix.data());
					
					
					if (ImGui::InputFloat3("Position", vectorPosition))
					{
						go3D->m_spMatrixNodeTransform->m_Transform.SetPosition(CVector(vectorPosition[0], vectorPosition[1], vectorPosition[2]));
						go3D->m_spMatrixNodeTransform->NotifyChanged();
					}
					if (ImGui::InputFloat3("Rotation", vectorRotation))
					{
						go3D->m_spMatrixNodeTransform->m_Transform.m_Rotation.x() = vectorRotation[0];
						go3D->m_spMatrixNodeTransform->m_Transform.m_Rotation.y() = vectorRotation[1];
						go3D->m_spMatrixNodeTransform->m_Transform.m_Rotation.z() = vectorRotation[2];
						go3D->m_spMatrixNodeTransform->NotifyChanged();
					}					
				}
			}
			
		}

		/// @separator
		ImGui::End();
	}
	vec1.clear();
	vec2.clear();
					
}
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
	void* rangeCullingMethod;
	void* dword4;
	void* frustrumCullingMethod;
	void* dwordC;
	void* changedInvokerCullingMethod;
	void* dword14;
	boost::shared_ptr<void> boost__void18;
	void* resetFlagCullingMethod;
	void* dword24;	
};

class CActivationManager : public Sonic::CGameObject
{
public:
	BYTE gap1[4];
	hh::map<int, boost::shared_ptr<ActivationElement>> possiblyAMap1;
	void* dwordB4;
	CActivationCullingManager* m_pActivationCullingManager;
	boost::shared_ptr<CActivationCullingManager> m_spActivationCullingManager;
	Hedgehog::Math::CQuaternion hedgehog__math__cquaternionC0;
	BYTE gapD0[4];
	void* dwordD4;
	void* dwordD8;
	__declspec(align(8)) void* dwordE0;
	hh::map<int, boost::shared_ptr<ActivationElement>> possiblyAMap2;
	Hedgehog::Universe::Message* msgGetCullingBasePosition;
	BYTE gapF0[8];
	void* dwordF8;
	void* dwordFC;
	BYTE byte100;
	void* dword104;
};
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
		for (auto it = member->m_Field1C.begin(); it != member->m_Field1C.end(); ++it)
		{
			auto first = *it;
			auto test = getTypeName(first);
			//auto test2 = getTypeName(((boost::shared_ptr<void*>)first).get());
		}
		auto activationmanager = (CActivationManager*)Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pActivationManager;
		for (auto it = t->m_mSetLayers.begin(); it != t->m_mSetLayers.end(); ++it)
		{
			auto one = it->first;
			auto two = it->second;
			auto three = it->second.get();//
			DebugDrawText::log(std::format("LAYER {0}\nName: {1}\nFilename: {2}\nSaving Location: {3}\ndwordC: {4}",one, three->m_LayerName.c_str(), three->m_FileName.c_str(), three->m_SaveLocation.c_str(), three->dwordC.c_str()).c_str());
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
				dynamic_cast<Sonic::CSetObjectListener*>(vec1.at(i).get())->CSetObjectListener20();
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
				dynamic_cast<Sonic::CSetObjectListener*>(vec1.at(i).get())->CSetObjectListener24();
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
void SetEditorTest::applyPatches()
{
	WRITE_JUMP(0x00FD6AB3, 0x00FD6ABF);
	//WRITE_JUMP(0x00EB681D, ASMTEST);
	//INSTALL_HOOK(ADD_SET_OBJECTS_TO_WORLD);
	INSTALL_HOOK(sub_597E50);
	INSTALL_HOOK(StartInstanceBrush);
	INSTALL_HOOK(CInstanceBrushDestructor);
	INSTALL_HOOK(sub_EA67D0);
	INSTALL_HOOK(SetUpdateApplication);
	INSTALL_HOOK(SetObjectManager_ProcessMessage);
}