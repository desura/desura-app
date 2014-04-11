#include "Common.h"
#include "wx_controls/gcDialog.h"
#include "wx_controls/gcButton.h"

#include "Log.h"
#include "AboutForm.h"
#include "LoginForm.h"
#include "MainMenuButton.h"
#include "ModWizard.h"
#include "ModWizardProgPage.h"
#include "CDKeyForm.h"
#include "CDKProgress.h"
#include "EulaForm.h"
#include "NewsForm.h"
#include "gcUpdateForm.h"
#include "NewAccountDialog.h"
#include "PasswordReminder.h"
#include "ExeSelectForm.h"
#include "ItemForm.h"
#include "usercore/ToolManagerI.h"
#include "CreateForm.h"
#include "CreateProgPage.h"
#include "UploadForm.h"
#include "UploadProgPage.h"
#include "UploadInfoPage.h"
#include "gcWebFakeBrowser.h"
#include "TaskBarIcon.h"
#include "DesuraServiceError.h"
#include "SteamUser.h"
#include "GameDiscForm.h"
#include "MainForm.h"
#include "gcJSBinding.h"
#include "usercore/UserCoreI.h"

using namespace UserCore::Item;
using namespace UI::Forms;

class LanguageTestDialog : public gcDialog 
{
public:
	LanguageTestDialog();

protected:
	void onButtonClicked(wxCommandEvent& event);
	void onChoice(wxCommandEvent& event);

private:
	wxButton *m_button1;
	wxButton *m_button2;
	wxChoice *m_choice1;
	wxScrolledWindow* m_scrolledWindow1;

	std::map<int, std::function<wxWindow*()>> m_ActionMap;
	std::vector<wxWindow*> m_vActiveFrames;

	std::vector<std::map<gcString, gcString>> m_vLanguages;
};



CONCOMMAND(cc_languagetest, "languagetest")
{
	LanguageTestDialog ltd;
	ltd.ShowModal();
}

class LanguageStubBranch : public UserCore::Item::BranchInfoI
{
public:
	virtual ~LanguageStubBranch(){}

	uint32 getFlags() override { return 0; }
	MCFBranch getBranchId() override { return MCFBranch(); }
	MCFBranch getGlobalId() override { return MCFBranch(); }
	DesuraId getItemId() override { return DesuraId(); }

	const char* getName() override { return ""; }
	const char* getCost() override { return ""; }
	const char* getEulaUrl() override { return "http://desura.com"; }


	const char* getPreOrderExpDate() override { return ""; }
	const char* getInstallScriptPath() override { return ""; }


	void getToolList(std::vector<DesuraId> &toolList) override { }

	bool isAvaliable() override { return true; }
	bool isDownloadable() override { return true; }
	bool isPreOrder() override { return m_bIsPreOrder; }
	bool isPreOrderAndNotPreload() override { return m_bIsPreOrder; }
	bool hasAcceptedEula() override { return true; }
	bool hasCDKey() override { return true; }
	bool isCDKeyValid() override { return true; }
	bool isWindows() override { return true; }
	bool isLinux() override { return true; }
	bool isMacOsX() override { return true; }
	bool is32Bit() override { return true; }
	bool is64Bit() override { return true; }
	bool isSteamGame() override { return m_bSteamGame; }

	bool m_bSteamGame = false;
	bool m_bIsPreOrder = false;
};

class LanguageStubExeInfo : public UserCore::Item::Misc::ExeInfoI
{
public:
	virtual ~LanguageStubExeInfo(){}

	const char* getName() override { return "[An Exe]"; }

	const char* getExe() override { return ""; }
	const char* getExeArgs() override { return ""; }
	const char* getUserArgs() override { return ""; }

	void setUserArgs(const char* args) override {}
};

class LanguageStubItem : public UserCore::Item::ItemInfoI
{
public:
	virtual ~LanguageStubItem(){}

	void updated() override { }
	void addToAccount() override { }
	void removeFromAccount() override { }

	DesuraId getParentId() override { return DesuraId(); }
	DesuraId getId() override { return m_Id; }
	DesuraId getInstalledModId(MCFBranch branch = MCFBranch()) override { return DesuraId(); }

	uint32 getChangedFlags() override { return 0; }
	uint32 getStatus() override { return m_uiStatus; }

	uint8 getPercent() override { return 0; }
	uint8 getPermissions() override { return 0; }
	uint8 getOptions() override { return 0; }

	bool isLaunchable() override { return m_bIsLaunchable; }
	bool isUpdating() override { return m_bIsUpdating; }
	bool isInstalled() override { return m_bIsInstalled; }
	bool isDownloadable() override { return m_bIsDownloadable; }
	bool isFavorite() override { return m_bIsFavorite; }
	bool isComplex() override { return false; }
	bool isParentToComplex() override { return false; }
	bool hasAcceptedEula() override { return false; }
	bool compare(const char* filter) override { return false; }
	bool isFirstLaunch() override { return false; }

	void addSFlag(uint32 status) override { }
	void addPFlag(uint8 permission) override { }
	void addOFlag(uint8 option) override { }
	void delSFlag(uint32 status) override { }
	void delPFlag(uint8 permission) override { }
	void delOFlag(uint8 option) override { }

	const char* getRating() override { return m_strRating; }
	const char* getDev() override { return m_strDev; }
	const char* getName() override { return m_strName.c_str(); }
	const char* getShortName() override { return m_strShortName; }
	const char* getPath(MCFBranch branch = MCFBranch()) override { return nullptr; }
	const char* getInsPrimary(MCFBranch branch = MCFBranch()) override { return nullptr; }
	const char* getIcon() override { return nullptr; }
	const char* getLogo() override { return nullptr; }
	const char* getIconUrl() override { return nullptr; }
	const char* getLogoUrl() override { return nullptr; }
	const char* getDesc() override { return nullptr; }
	const char* getTheme() override { return nullptr; }
	const char* getGenre() override { return nullptr; }
	const char* getProfile() override { return nullptr; }
	const char* getDevProfile() override { return nullptr; }
	const char* getPublisher() override { return m_strPublisher; }
	const char* getPublisherProfile() override { return nullptr; }
	const char* getEulaUrl() override { return nullptr; }
	const char* getInstallScriptPath() override { return nullptr; }

	Event<ItemInfo_s>* getInfoChangeEvent() override { return &m_ChangeEvent; }
	uint32 getBranchCount() override { return 1; }
	UserCore::Item::BranchInfoI* getBranch(uint32 index) override { return &m_Branch; }
	UserCore::Item::BranchInfoI* getCurrentBranch() override 
	{ 
		if (m_bNullCurBranch)
			return nullptr;

		return &m_Branch; 
	}
	UserCore::Item::BranchInfoI* getBranchById(uint32 id) override { return &m_Branch; }


	uint64 getInstallSize(MCFBranch branch = MCFBranch()) override { return 0; }
	uint64 getDownloadSize(MCFBranch branch = MCFBranch()) override { return 0; }

	MCFBuild getLastInstalledBuild(MCFBranch branch = MCFBranch()) override { return MCFBuild(); }
	MCFBuild getInstalledBuild(MCFBranch branch = MCFBranch()) override { return MCFBuild(); }
	MCFBuild getNextUpdateBuild(MCFBranch branch = MCFBranch()) override { return MCFBuild(); }
	MCFBranch getInstalledBranch() override { return MCFBranch(); }
	MCFBranch getLastInstalledBranch() override { return MCFBranch(); }

	const char* getInstalledVersion(MCFBranch branch = MCFBranch()) override { return "V1"; }

	bool setInstalledMcf(MCFBranch branch, MCFBuild build = MCFBuild()) override { return true; }
	void setFavorite(bool fav = true) override { }
	void acceptEula() override { }
	uint32 getExeCount(bool setActive = false, MCFBranch branch = MCFBranch()) override { return 0; }

	void getExeList(std::vector<UserCore::Item::Misc::ExeInfoI*> &list, MCFBranch branch = MCFBranch()) override 
	{ 
		list.push_back(&m_Exe1);
		list.push_back(&m_Exe2);
	}

	UserCore::Item::Misc::ExeInfoI* getActiveExe(MCFBranch branch = MCFBranch()) override { return &m_Exe1; }
	void setActiveExe(const char* name, MCFBranch branch = MCFBranch()) override { }

	ItemInfoInternalI* getInternal() override { return nullptr; }

	LanguageStubExeInfo m_Exe1;
	LanguageStubExeInfo m_Exe2;
	LanguageStubBranch m_Branch;

	bool m_bNullCurBranch = false;

	Event<ItemInfo_s> m_ChangeEvent;

	bool m_bIsLaunchable = true;
	bool m_bIsUpdating = false;
	bool m_bIsInstalled = false;
	bool m_bIsDownloadable = true;
	bool m_bIsFavorite = false;

	const char* m_strRating = "0";
	const char* m_strDev = "[Test Dev]";
	const char* m_strPublisher = "[Test Publisher]";
	gcString m_strName = "[Test Game]";
	const char* m_strShortName = "TEST";

	uint32 m_uiStatus = 0;

	DesuraId m_Id;
};

class LanguageStubToolManager : public UserCore::ToolManagerI
{
public:
	virtual ~LanguageStubToolManager(){}

	void removeTransaction(ToolTransactionId ttid, bool forced) override { }
	ToolTransactionId downloadTools(UserCore::Misc::ToolTransaction* transaction) override { return ToolTransactionId(); }
	ToolTransactionId installTools(UserCore::Misc::ToolTransaction* transaction) override { return ToolTransactionId(); }
	bool updateTransaction(ToolTransactionId ttid, UserCore::Misc::ToolTransaction* transaction) override { return false; }
	void parseXml(const XML::gcXMLElement &toolinfoNode) override { }
	bool areAllToolsValid(const std::vector<DesuraId> &list) override { return false; }
	bool areAllToolsDownloaded(const std::vector<DesuraId> &list) override { return false; }
	bool areAllToolsInstalled(const std::vector<DesuraId> &list) override { return false; }
	void saveItems() override { }
	std::string getToolName(DesuraId toolId) override { return "[Test Tool]"; }
	void findJSTools(UserCore::Item::ItemInfo* item) override { }
	bool initJSEngine() override { return true; }
	void destroyJSEngine() override { }
	void invalidateTools(std::vector<DesuraId> &list) override { }
		
#ifdef NIX
	void symLinkTools(std::vector<DesuraId> &list, const char* path) override { }
	int hasNonInstallableTool(std::vector<DesuraId> &list) override { return 0; }
#endif	

	void reloadTools(DesuraId id) override {}
};

class LanguageStubItemHandle : public UserCore::Item::ItemHandleI, public UserCore::Item::ItemTaskGroupI
{
public:
	virtual ~LanguageStubItemHandle(){}

	void setFactory(Helper::ItemHandleFactoryI* factory) override { }
	void addHelper(Helper::ItemHandleHelperI* helper) override 
	{
		gcAssert(m_pHelper == nullptr);
		m_pHelper = helper;
	}

	void delHelper(Helper::ItemHandleHelperI* helper) override 
	{
		if (m_pHelper == helper)
			m_pHelper = nullptr;
	}

	bool cleanComplexMods() override { return true; }
	bool verify(bool files, bool tools, bool hooks) override { return true; }
	bool update() override { return true; }
	bool install(Helper::ItemLaunchHelperI* helper, MCFBranch branch) override { return true; }
	bool install(MCFBranch branch, MCFBuild build, bool test = false) override { return true; }
	bool installCheck() override { return true; }
	bool launch(Helper::ItemLaunchHelperI* helper, bool offline = false, bool ignoreUpdate = false) override { return true; }
	bool switchBranch(MCFBranch branch) override { return true; }
	bool startUpCheck() override { return true; }
	bool uninstall(Helper::ItemUninstallHelperI* helper, bool complete, bool account) override { return true; }

	void setPaused(bool paused = true) override { }
	void setPauseOnError(bool pause = true) override { }
	bool shouldPauseOnError() override { return false; }
	bool isStopped() override { return false; }
	bool isInStage() override { return false; }

	ITEM_STAGE getStage() override { return ITEM_STAGE::STAGE_NONE; }
	void cancelCurrentStage() override { }

	UserCore::Item::ItemInfoI* getItemInfo() override { return m_pItemInfo; }

	Event<ITEM_STAGE>* getChangeStageEvent() override { return &m_ChangeStageEvent; }
	Event<gcException>* getErrorEvent() override { return &m_ErrorEvent; }

	void getStatusStr(LanguageManagerI & pLangMng, char* buffer, uint32 buffsize) override 
	{ 
		static GetStatusStrFn s_GetStatusStr = (GetStatusStrFn)UserCore::FactoryBuilderUC(USERCORE_GETITEMSTATUS);

		if (s_GetStatusStr)
			s_GetStatusStr(this, m_pItemInfo, m_Stage, this, pLangMng, buffer, buffsize);
	}

	ItemTaskGroupI* getTaskGroup() override { return nullptr; }

	void force() override { }

	bool createDesktopShortcut() override { return true; }
	bool createMenuEntry() override { return true; }

#ifdef NIX
	void installLaunchScripts() override { }
#endif

	
	ItemHandleInternalI* getInternal() override { return nullptr; }

	//Item Group
	ItemTaskGroupI::ACTION getAction() override { return ItemTaskGroupI::A_VERIFY; }
	void getItemList(std::vector<UserCore::Item::ItemHandleI*> &list) override { }
	void cancelAll() override { }

	uint32 getPos(UserCore::Item::ItemHandleI* item) override { return 1; }
	uint32 getCount() override { return 2; }


	ITEM_STAGE m_Stage = ITEM_STAGE::STAGE_NONE;

	Event<ITEM_STAGE> m_ChangeStageEvent;
	Event<gcException> m_ErrorEvent;

	UserCore::Item::ItemInfoI *m_pItemInfo = nullptr;
	Helper::ItemHandleHelperI* m_pHelper = nullptr;
};

class LangStubItemManager : public UserCore::ItemManagerI
{
public:
	LangStubItemManager()
	{
		m_ItemHandle.m_pItemInfo = &m_Item;
	}

	virtual ~LangStubItemManager(){}

	void loadItems() override { }
	void saveItems() override { }

	bool isInstalled(DesuraId id) override { return true; }
	void removeItem(DesuraId id) override { }

	void retrieveItemInfo(DesuraId id, uint32 statusOveride = 0, WildcardManager* pWildCard = nullptr, MCFBranch mcfBranch = MCFBranch(), MCFBuild mcfBuild = MCFBuild(), bool reset = false) override { }
	void retrieveItemInfoAsync(DesuraId id, bool addToAccount = false) override { }

	uint32 getDevItemCount() override { return 0; }

	UserCore::Item::ItemInfoI* findItemInfo(DesuraId id) override { return &m_Item; }
	UserCore::Item::ItemHandleI* findItemHandle(DesuraId ide) override { return &m_ItemHandle; }

	uint32 getCount() override { return 1; }

	UserCore::Item::ItemInfoI* getItemInfo(uint32 index) override { return &m_Item; }
	UserCore::Item::ItemHandleI* getItemHandle(uint32 index) override { return &m_ItemHandle; }

	void getCIP(DesuraId id, char** buff) override { }
	void getAllItems(std::vector<UserCore::Item::ItemInfoI*> &aList) override { }
	void getGameList(std::vector<UserCore::Item::ItemInfoI*> &gList, bool includeDeleted = false) override { }
	void getModList(DesuraId gameId, std::vector<UserCore::Item::ItemInfoI*> &mList, bool includeDeleted = false) override { }
	void getDevList(std::vector<UserCore::Item::ItemInfoI*> &dList) override { }
	void getFavList(std::vector<UserCore::Item::ItemInfoI*> &fList) override { }
	void getRecentList(std::vector<UserCore::Item::ItemInfoI*> &rList) override { }
	void getLinkList(std::vector<UserCore::Item::ItemInfoI*> &lList) override { }
	void getNewItems(std::vector<UserCore::Item::ItemInfoI*> &tList) override { }

	void itemsNeedUpdate(const XML::gcXMLElement &itemsNode) override { }
	void itemsNeedUpdate2(const XML::gcXMLElement &platformsNode) override { }

	void setFavorite(DesuraId id, bool fav) override { }
	void setRecent(DesuraId id) override { }
	void setInstalledMod(DesuraId parentId, DesuraId modId) override { }
	void checkItems() override { }

	EventV* getOnUpdateEvent() override { return nullptr; }
	Event<DesuraId>* getOnRecentUpdateEvent() override { return nullptr; }
	Event<DesuraId>* getOnFavoriteUpdateEvent() override { return nullptr; }
	Event<DesuraId>* getOnNewItemEvent() override { return nullptr; }


	DesuraId addLink(const char* name, const char* exe, const char* args) override { return DesuraId(); }

	void updateLink(DesuraId id, const char* args) override { }
	bool isKnownBranch(MCFBranch branch, DesuraId id) override { return false; }
	bool isItemFavorite(DesuraId id) override { return false; }
	void regenLaunchScripts() override { }

	void saveItem(UserCore::Item::ItemInfoI *) override { }

	LanguageStubItem m_Item;
	LanguageStubItemHandle m_ItemHandle;
};

//0. Not installed
//1. Not Downloadable && installed
//2. Not Downloadable && Not Installed

//3. UserCore::Item::ITEM_STAGE::STAGE_DOWNLOADTOOL
//4. UserCore::Item::ITEM_STAGE::STAGE_INSTALLTOOL
//5. UserCore::Item::ITEM_STAGE::STAGE_LAUNCH
//6. UserCore::Item::ITEM_STAGE::STAGE_VALIDATE

//7. UserCore::Item::ItemInfoI::STATUS_VERIFING
//8. UserCore::Item::ItemInfoI::STATUS_UPDATING
//9. UserCore::Item::ItemInfoI::STATUS_DOWNLOADING
//10. UserCore::Item::ItemInfoI::STATUS_INSTALLING
//11. UserCore::Item::ItemInfoI::STATUS_INSTALLING & UserCore::Item::ItemInfoI::STATUS_PAUSED
//12. UserCore::Item::ItemInfoI::STATUS_READY
//13. UserCore::Item::ItemInfoI::STATUS_READY & UserCore::Item::ItemInfoI::STATUS_PRELOADED
//14. UserCore::Item::ItemInfoI::STATUS_DEVELOPER

//15. UserCore::Item::ITEM_STAGE::STAGE_WAIT with task group
//16. null cur branch & has preorder branch

class LangPlayItemManager : public UserCore::ItemManagerI
{
public:
	LangPlayItemManager()
	{
		for (int x = 0; x < 17; x++)
		{
			m_vItems.push_back(LanguageStubItem());
			m_vItemHandle.push_back(LanguageStubItemHandle());
		}

		for (int x = 0; x < 17; x++)
		{
			m_vItems[x].m_Id = DesuraId(x+1, DesuraId::TYPE_GAME);
			m_vItems[x].m_strName = gcString("[Test Game {0:2}]", x);

			m_vItemHandle[x].m_pItemInfo = &m_vItems[x];
		}

		m_vItems[1].m_bIsDownloadable = false;

		m_vItems[2].m_bIsInstalled = true;
		m_vItems[2].m_bIsDownloadable = false;

		m_vItemHandle[3].m_Stage = ITEM_STAGE::STAGE_DOWNLOADTOOL;
		m_vItemHandle[4].m_Stage = ITEM_STAGE::STAGE_INSTALLTOOL;
		m_vItemHandle[5].m_Stage = ITEM_STAGE::STAGE_LAUNCH;
		m_vItemHandle[6].m_Stage = ITEM_STAGE::STAGE_VALIDATE;

		m_vItems[7].m_uiStatus = ItemInfoI::STATUS_VERIFING;
		m_vItems[8].m_uiStatus = ItemInfoI::STATUS_UPDATING;
		m_vItems[9].m_uiStatus = ItemInfoI::STATUS_DOWNLOADING;
		m_vItems[10].m_uiStatus = ItemInfoI::STATUS_INSTALLING;
		m_vItems[11].m_uiStatus = ItemInfoI::STATUS_INSTALLING | ItemInfoI::STATUS_PAUSED;
		m_vItems[12].m_uiStatus = ItemInfoI::STATUS_READY;

		m_vItems[13].m_uiStatus = ItemInfoI::STATUS_READY | ItemInfoI::STATUS_PRELOADED; //cur branch preloaded
		m_vItems[13].m_Branch.m_bIsPreOrder = true;

		m_vItems[14].m_uiStatus = ItemInfoI::STATUS_DEVELOPER;

		m_vItemHandle[15].m_Stage = ITEM_STAGE::STAGE_WAIT;

		m_vItems[16].m_bNullCurBranch = true;
		m_vItems[16].m_Branch.m_bIsPreOrder = true;
	}

	virtual ~LangPlayItemManager(){}

	void loadItems() override { }
	void saveItems() override { }

	bool isInstalled(DesuraId id) override { return true; }
	void removeItem(DesuraId id) override { }

	void retrieveItemInfo(DesuraId id, uint32 statusOveride = 0, WildcardManager* pWildCard = nullptr, MCFBranch mcfBranch = MCFBranch(), MCFBuild mcfBuild = MCFBuild(), bool reset = false) override { }
	void retrieveItemInfoAsync(DesuraId id, bool addToAccount = false) override { }

	uint32 getDevItemCount() override { return 0; }

	UserCore::Item::ItemInfoI* findItemInfo(DesuraId id) override 
	{ 
		auto it = std::find_if(begin(m_vItems), end(m_vItems), [id](LanguageStubItem& item){
			return item.getId() == id;
		});

		if (it == end(m_vItems))
			return nullptr;

		return &*it;
	}

	UserCore::Item::ItemHandleI* findItemHandle(DesuraId id) override 
	{ 
		auto it = std::find_if(begin(m_vItemHandle), end(m_vItemHandle), [id](LanguageStubItemHandle& handle){
			return handle.m_pItemInfo->getId() == id;
		});

		if (it == end(m_vItemHandle))
			return nullptr;

		return &*it;
	}

	uint32 getCount() override { return m_vItems.size(); }

	UserCore::Item::ItemInfoI* getItemInfo(uint32 index) override { return &m_vItems[index]; }
	UserCore::Item::ItemHandleI* getItemHandle(uint32 index) override { return &m_vItemHandle[index]; }

	void getCIP(DesuraId id, char** buff) override { }

	void getAllItems(std::vector<UserCore::Item::ItemInfoI*> &aList) override 
	{
		for (auto& i : m_vItems)
			aList.push_back(&i);
	}

	void getGameList(std::vector<UserCore::Item::ItemInfoI*> &gList, bool includeDeleted = false) override 
	{
		return getAllItems(gList);
	}

	void getModList(DesuraId gameId, std::vector<UserCore::Item::ItemInfoI*> &mList, bool includeDeleted = false) override { }
	void getDevList(std::vector<UserCore::Item::ItemInfoI*> &dList) override 
	{
		return getAllItems(dList);
	}

	void getFavList(std::vector<UserCore::Item::ItemInfoI*> &fList) override 
	{
		return getAllItems(fList);
	}

	void getRecentList(std::vector<UserCore::Item::ItemInfoI*> &rList) override 
	{
		return getAllItems(rList);
	}

	void getLinkList(std::vector<UserCore::Item::ItemInfoI*> &lList) override { }
	void getNewItems(std::vector<UserCore::Item::ItemInfoI*> &tList) override 
	{
		return getAllItems(tList);
	}


	void itemsNeedUpdate(const XML::gcXMLElement &itemsNode) override { }
	void itemsNeedUpdate2(const XML::gcXMLElement &platformsNode) override { }

	void setFavorite(DesuraId id, bool fav) override { }
	void setRecent(DesuraId id) override { }
	void setInstalledMod(DesuraId parentId, DesuraId modId) override { }
	void checkItems() override { }

	EventV* getOnUpdateEvent() override { return nullptr; }
	Event<DesuraId>* getOnRecentUpdateEvent() override { return nullptr; }
	Event<DesuraId>* getOnFavoriteUpdateEvent() override { return nullptr; }
	Event<DesuraId>* getOnNewItemEvent() override { return nullptr; }


	DesuraId addLink(const char* name, const char* exe, const char* args) override { return DesuraId(); }

	void updateLink(DesuraId id, const char* args) override { }
	bool isKnownBranch(MCFBranch branch, DesuraId id) override { return false; }
	bool isItemFavorite(DesuraId id) override { return false; }
	void regenLaunchScripts() override { }

	void saveItem(UserCore::Item::ItemInfoI *) override { }

	std::vector<LanguageStubItem> m_vItems;
	std::vector<LanguageStubItemHandle> m_vItemHandle;
};




extern std::vector<std::map<gcString, gcString>> GetLanguages();


class LangBrowserWindow : public gcFrame
{
public:
	LangBrowserWindow(wxWindow* parent, const char* szUrl, bool bUseFake = false)
		: gcFrame(parent, wxID_ANY, szUrl, wxDefaultPosition, wxSize(500, 500))
	{
		if (bUseFake)
			m_pBrowser = new gcWebControl(this, szUrl, &CreateFakeBrowser);
		else
			m_pBrowser = new gcWebControl(this, szUrl);

		wxFlexGridSizer* fgSizer2 = new wxFlexGridSizer(2, 1, 0, 0);
		fgSizer2->AddGrowableCol(0);
		fgSizer2->AddGrowableRow(0);
		fgSizer2->SetFlexibleDirection(wxBOTH);
		fgSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
		fgSizer2->Add(m_pBrowser, 1, wxEXPAND | wxTOP | wxRIGHT | wxLEFT, 5);

		this->SetSizer(fgSizer2);
		this->Layout();
	}

	static ChromiumDLL::ChromiumBrowserI* CreateFakeBrowser(gcWebControl *pControl, const char* loadUrl)
	{
		return new gcWebFakeBrowser(pControl);
	}

private:
	gcWebControl *m_pBrowser;
};

LanguageTestDialog::LanguageTestDialog() 
	: gcDialog(nullptr, wxID_ANY, "Language Test Dialog", wxDefaultPosition, wxSize(350, 500))
{
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LanguageTestDialog::onButtonClicked, this);

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	m_vLanguages = GetLanguages();

	wxArrayString m_choice1Choices;

	m_choice1Choices.Add("None");

	for (auto l : m_vLanguages)
		m_choice1Choices.Add(l["name"]);

	m_choice1 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1Choices, 0 );
	m_choice1->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &LanguageTestDialog::onChoice, this);

	m_button1 = new wxButton( this, wxID_ANY, wxT("Show All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button2 = new wxButton( this, wxID_ANY, wxT("Close All"), wxDefaultPosition, wxDefaultSize, 0 );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	bSizer1->Add( m_choice1, 0, wxALL, 5 );
	bSizer1->Add( m_button1, 0, wxALL, 5 );
	bSizer1->Add( m_button2, 0, wxALL, 5 );


	m_scrolledWindow1 = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow1->SetScrollRate( 5, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	bSizer3->Fit( m_scrolledWindow1 );

	m_scrolledWindow1->SetSizer( bSizer3 );
	m_scrolledWindow1->Layout();
	
	wxFlexGridSizer* gSizer1;
	gSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	gSizer1->AddGrowableRow( 1 );
	gSizer1->AddGrowableCol( 0 );
	gSizer1->SetFlexibleDirection( wxBOTH );
	gSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	gSizer1->Add( bSizer1, 1, wxEXPAND, 5 );
	gSizer1->Add( m_scrolledWindow1, 1, wxEXPAND | wxALL, 5 );


	auto addButton = [&](const char* szName, std::function<wxWindow*()> disp)
	{
		auto button = new gcButton( m_scrolledWindow1, wxID_ANY, szName, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer3->Add( button, 0, wxALL|wxEXPAND, 5 );

		m_ActionMap[button->GetId()] = disp;
	};

	addButton("About Dialog", [this](){
		auto af = new AboutForm(this);
		af->Show();
		return af;
	});

	addButton("Login Dialog", [this](){
		auto af = new LoginForm(this);
		af->m_bDisabled = true;
		af->Show();
		return af;
	});

	addButton("Login Error Generic", [this](){
		LoginForm af(this);
		af.newAccountLoginError("We had a login error");

		return nullptr;
	});
	
	addButton("Login Error Empty Username", [this](){
		LoginForm af(this);
		af.doLogin("", "abc");
		return nullptr;
	});

	addButton("Login Error Empty Password", [this](){
		LoginForm af(this);
		af.doLogin("abc", "");
		return nullptr;
	});

	addButton("Console", [this](){
		ShowLogForm(true);
		return nullptr;
	});

	addButton("Main Menu - Offline", [this](){
		MainMenuButton but(this, true);

		wxCommandEvent ev;
		but.onMouseClick(ev);
		return nullptr;
	});

	addButton("Main Menu - Online", [this](){
		MainMenuButton but(this, false);

		wxCommandEvent ev;
		but.onMouseClick(ev);
		return nullptr;
	});

	addButton("Games Wizard - Info", [this](){
		auto mw = new ModWizardForm(this);

		mw->m_bDisabled = true;
		mw->Show();

		return mw;
	});

	addButton("Games Wizard - Progress", [this](){
		auto mw = new ModWizardForm(this);

		mw->m_bDisabled = true;
		mw->run(false);
		mw->Show();

		return mw;
	});

	addButton("Games Wizard - Done", [this](){
		auto mw = new ModWizardForm(this);

		mw->m_bDisabled = true;
		mw->finish();
		mw->Show();

		return mw;
	});

	addButton("Games Wizard - Error", [this](){
		auto mw = new ModWizardForm(this);

		mw->m_bDisabled = true;
		mw->run(false);
		mw->Show();

		gcException e;
		dynamic_cast<MWProgressPage*>(mw->m_pPage)->onError(e);

		return mw;
	});


	addButton("CD Key - Info", [this](){
		auto itemMang = new LangStubItemManager();
		auto mw = new CDKeyForm(this, "", false, itemMang);
		mw->Show();
		return mw;
	});

	addButton("CD Key - Display Launch", [this](){
		auto itemMang = new LangStubItemManager();
		itemMang->m_Item.m_Branch.m_bSteamGame = false;

		auto mw = new CDKeyForm(this, "", true, itemMang);
		mw->finish("ABCD-1234-EFGH-5678-IJKL");
		mw->Show();
		return mw;
	});

	addButton("CD Key - Display Steam", [this](){
		auto itemMang = new LangStubItemManager();
		itemMang->m_Item.m_Branch.m_bSteamGame = true;

		auto mw = new CDKeyForm(this, "", false, itemMang);
		mw->finish("ABCD-1234-EFGH-5678-IJKL");
		mw->Show();
		return mw;
	});

	addButton("CD Key - Display", [this](){
		auto itemMang = new LangStubItemManager();
		itemMang->m_Item.m_Branch.m_bSteamGame = false;

		auto mw = new CDKeyForm(this, "", false, itemMang);
		mw->finish("ABCD-1234-EFGH-5678-IJKL");
		mw->Show();
		return mw;
	});

	addButton("CD Key - Error", [this](){
		auto itemMang = new LangStubItemManager();
		auto mw = new CDKeyForm(this, "", false, itemMang);
		mw->Show();

		gcException e;
		dynamic_cast<CDKProgress*>(mw->m_pPage)->onError(e);
		return mw;
	});


	addButton("Eula", [this](){
		auto itemMang = new LangStubItemManager();
		auto form = new EULAForm(this, itemMang);
		form->setInfo(DesuraId());
		form->Show();
		return form;
	});

	std::vector<UserCore::Misc::NewsItem> vNews = 
	{
		UserCore::Misc::NewsItem(0, 0, "Test News 1", "http://www.desura.com/games"),
		UserCore::Misc::NewsItem(0, 0, "Test News 2", "http://www.desura.com/mods")
	};


	addButton("News", [this, vNews](){
		auto form = new NewsForm(this);
		form->loadNewsItems(vNews);
		form->Show();
		return form;
	});

	addButton("Gifts", [this, vNews](){
		auto form = new NewsForm(this);

		form->setAsGift();
		form->loadNewsItems(vNews);

		form->Show();
		return form;
	});

	addButton("Desura Update", [this](){
		auto form = new GCUpdateInfo(this);
		form->Show();
		return form;
	});

	
	addButton("New Account", [this](){
		auto form = new NewAccountDialog(this);
		form->Show();
		return form;
	});


	addButton("Password Reminder - Info", [this]()
	{
		auto form = new PasswordReminder(this);
		form->Show();
		return form;
	});

	addButton("Password Reminder - Sent", [this](){
		auto form = new PasswordReminder(this);
		form->Show();
		form->onComplete();
		return form;
	});

	addButton("Password Reminder - Error Generic", [this](){
		auto form = new PasswordReminder(this);
		form->Show();
		gcException e;
		form->onError(e);
		return form;
	});

	addButton("Password Reminder - Error Email", [this](){
		auto form = new PasswordReminder(this);
		form->Show();
		wxCommandEvent e;
		e.SetId(form->m_butSend->GetId());
		form->onButtonClicked(e);
		return form;
	});



	addButton("Exe Select Form", [this](){
		auto itemMang = new LangStubItemManager();
		auto form = new ExeSelectForm(this, true, itemMang);
		form->setInfo(DesuraId());
		form->Show();
		return form;
	});
	

	auto setupAction = [this](INSTALL_ACTION action)
	{
		auto itemMang = new LangStubItemManager();
		auto form = new ItemForm(this);
		form->init(action, MCFBranch(), MCFBuild(), true, &itemMang->m_ItemHandle);
		form->Show();
		return form;
	};

	auto setupActionWithInfo = [this](INSTALL_ACTION action, const char* actionStr, const char* id)
	{
		auto itemMang = new LangStubItemManager();
		auto form = new ItemForm(this, actionStr, id);
		form->init(action, MCFBranch(), MCFBuild(), true, &itemMang->m_ItemHandle);
		form->Show();
		return form;
	};

	auto setupActionWithStage = [this](INSTALL_ACTION action, ITEM_STAGE stage)
	{
		auto itemMang = new LangStubItemManager();
		auto form = new ItemForm(this);
		form->init(action, MCFBranch(), MCFBuild(), true, &itemMang->m_ItemHandle);
		
		itemMang->m_ItemHandle.m_ChangeStageEvent(stage);
		form->Show();
		return form;
	};

	auto setupActionWithStageAndHelper = [this](INSTALL_ACTION action, ITEM_STAGE stage, Helper::ItemHandleHelperI** helper)
	{
		auto itemMang = new LangStubItemManager();
		auto toolMang = new LanguageStubToolManager();

		auto form = new ItemForm(this);
		form->m_pToolManager = toolMang;

		form->init(action, MCFBranch(), MCFBuild(), true, &itemMang->m_ItemHandle);
		
		itemMang->m_ItemHandle.m_ChangeStageEvent(stage);
		form->Show();

		if (helper)
			*helper = itemMang->m_ItemHandle.m_pHelper;

		return form;
	};

	addButton("Uninstall - Info", [this, setupAction](){
		return setupAction(INSTALL_ACTION::IA_UNINSTALL);
	});

	addButton("Uninstall - Progress", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto ret = setupActionWithStageAndHelper(INSTALL_ACTION::IA_UNINSTALL, ITEM_STAGE::STAGE_UNINSTALL, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.percent = 25;
		pHelper->onMcfProgress(info);

		return ret;
	});

	addButton("Uninstall - Progress Patch", [this, setupActionWithStage](){
		return setupActionWithStage(INSTALL_ACTION::IA_UNINSTALL, ITEM_STAGE::STAGE_UNINSTALL_PATCH);
	});

	addButton("Uninstall - Progress Branch", [this, setupActionWithStage](){
		return setupActionWithStage(INSTALL_ACTION::IA_UNINSTALL, ITEM_STAGE::STAGE_UNINSTALL_BRANCH);
	});

	addButton("Uninstall - Progress Complex", [this, setupActionWithStage](){
		return setupActionWithStage(INSTALL_ACTION::IA_UNINSTALL, ITEM_STAGE::STAGE_UNINSTALL_COMPLEX);
	});

	addButton("Uninstall - Progress Update", [this, setupActionWithStage](){
		return setupActionWithStage(INSTALL_ACTION::IA_UNINSTALL, ITEM_STAGE::STAGE_UNINSTALL_UPDATE);
	});


	addButton("Uninstall - Complete", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto ret = setupActionWithStageAndHelper(INSTALL_ACTION::IA_UNINSTALL, ITEM_STAGE::STAGE_UNINSTALL, &pHelper);

		uint32 e=0;
		pHelper->onComplete(e);

		return ret;
	});
	

	addButton("Uninstall - Error", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto ret = setupActionWithStageAndHelper(INSTALL_ACTION::IA_UNINSTALL, ITEM_STAGE::STAGE_UNINSTALL, &pHelper);

		gcException e;
		pHelper->onError(e);

		return ret;
	});
	


	addButton("Verify - Info", [this, setupAction](){
		return setupAction(INSTALL_ACTION::IA_VERIFY);
	});

	addButton("Verify - Progress Stage 0", [this, setupActionWithStage](){
		return setupActionWithStage(INSTALL_ACTION::IA_VERIFY, ITEM_STAGE::STAGE_VERIFY);
	});


	addButton("Verify - Progress Stage 1", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto ret = setupActionWithStageAndHelper(INSTALL_ACTION::IA_VERIFY, ITEM_STAGE::STAGE_VERIFY, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = 1;

		pHelper->onMcfProgress(info);
		return ret;
	});

	addButton("Verify - Progress Stage 2", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto ret = setupActionWithStageAndHelper(INSTALL_ACTION::IA_VERIFY, ITEM_STAGE::STAGE_VERIFY, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = 2;

		pHelper->onMcfProgress(info);
		return ret;
	});

	addButton("Verify - Progress Stage 3", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto ret = setupActionWithStageAndHelper(INSTALL_ACTION::IA_VERIFY, ITEM_STAGE::STAGE_VERIFY, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = 3;
		info.doneAmmount = 1000;
		info.totalAmmount = 10000;

		pHelper->onMcfProgress(info);
		return ret;
	});

	addButton("Verify - Progress Stage 4", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto ret = setupActionWithStageAndHelper(INSTALL_ACTION::IA_VERIFY, ITEM_STAGE::STAGE_VERIFY, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = 4;

		pHelper->onMcfProgress(info);
		return ret;
	});

	addButton("Verify - Progress Stage 5", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto ret = setupActionWithStageAndHelper(INSTALL_ACTION::IA_VERIFY, ITEM_STAGE::STAGE_VERIFY, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = 5;

		pHelper->onMcfProgress(info);
		return ret;
	});

	addButton("Verify - Complete", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto ret = setupActionWithStageAndHelper(INSTALL_ACTION::IA_VERIFY, ITEM_STAGE::STAGE_VERIFY, &pHelper);

		UserCore::Misc::VerifyComplete info;
		pHelper->onVerifyComplete(info);
		return ret;
	});

	addButton("Verify - Error", [this, setupActionWithStageAndHelper](){

		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto ret = setupActionWithStageAndHelper(INSTALL_ACTION::IA_VERIFY, ITEM_STAGE::STAGE_VERIFY, &pHelper);

		gcException e;
		pHelper->onError(e);

		return ret;
	});


	addButton("Launch - Offline", [this, setupAction](){
		ItemForm::showLaunchError();
		return nullptr;
	});

	addButton("Launch - Waiting", [this, setupActionWithInfo](){
		return setupActionWithInfo(INSTALL_ACTION::IA_LAUNCH, "Launch", "[Test Item]");
	});



	addButton("Install - GatherInfo", [this, setupActionWithStage](){
		return setupActionWithStage(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_GATHERINFO);
	});


	addButton("Install - GatherInfo - Prog Stage 2", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_GATHERINFO, &pHelper);

		uint32 prog = 45;
		pHelper->onProgressUpdate(prog);

		return res;
	});

	addButton("Install - GatherInfo - Error", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_GATHERINFO, &pHelper);

		gcException e;
		pHelper->onError(e);

		return res;
	});

	addButton("Install - GatherInfo - Error Platform", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_GATHERINFO, &pHelper);

		gcException e(ERR_UNSUPPORTEDPLATFORM);
		pHelper->onError(e);

		return res;
	});

	addButton("Install - GatherInfo - Error Branch", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_GATHERINFO, &pHelper);

		gcException e(ERR_UNSUPPORTEDPLATFORM, 123);
		pHelper->onError(e);

		return res;
	});




	addButton("Install - Download Init", [this, setupActionWithStage](){
		return setupActionWithStage(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_DOWNLOAD);
	});

	addButton("Install - Download Prog", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_DOWNLOAD, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = MCFCore::Misc::ProgressInfo::FLAG_INITFINISHED;
		pHelper->onMcfProgress(info);

		info.flag = 0;
		info.hour = 1;
		info.min = 2;
		info.rate = 245;
		info.percent = 24;
		info.totalAmmount = 123;
		info.doneAmmount = 1;
		pHelper->onMcfProgress(info);

		return res;
	});

	addButton("Install - Download Finished", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_DOWNLOAD, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = MCFCore::Misc::ProgressInfo::FLAG_FINALIZING;
		pHelper->onMcfProgress(info);


		return res;
	});

	addButton("Install - Download Prog Paused", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_DOWNLOAD, &pHelper);

		pHelper->onPause(true);
		return res;
	});

	addButton("Install - Download Error", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_DOWNLOAD, &pHelper);

		gcException e;
		pHelper->onError(e);

		return res;
	});




	addButton("Install - Update", [this, setupAction](){
		return setupAction(INSTALL_ACTION::IA_UPDATE);
	});

	addButton("Install - Install", [this, setupActionWithStage](){
		return setupActionWithStage(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_INSTALL);
	});

	addButton("Install - Install Prog", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_INSTALL, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = MCFCore::Misc::ProgressInfo::FLAG_INITFINISHED;
		pHelper->onMcfProgress(info);

		info.flag = 0;
		info.hour = 1;
		info.min = 2;
		info.rate = 245;
		info.percent = 24;
		info.totalAmmount = 123;
		info.doneAmmount = 1;
		pHelper->onMcfProgress(info);

		return res;
	});

	addButton("Install - Install Prog Paused", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_INSTALL, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = MCFCore::Misc::ProgressInfo::FLAG_INITFINISHED;
		pHelper->onMcfProgress(info);

		info.flag = 0;
		info.hour = 1;
		info.min = 2;
		info.rate = 245;
		info.percent = 24;
		info.totalAmmount = 123;
		info.doneAmmount = 1;
		pHelper->onMcfProgress(info);

		pHelper->onPause(true);
		return res;
	});

	addButton("Install - Install Complete", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_INSTALL, &pHelper);

		uint32 c = 0;
		pHelper->onComplete(c);
		return res;
	});

	addButton("Install - Install Complete Preorder", [this](){

		auto itemMang = new LangStubItemManager();
		auto form = new ItemForm(this);
		form->init(INSTALL_ACTION::IA_INSTALL, MCFBranch(), MCFBuild(), true, &itemMang->m_ItemHandle);
		
		auto stage = ITEM_STAGE::STAGE_INSTALL;
		itemMang->m_ItemHandle.m_ChangeStageEvent(stage);
		form->Show();
		
		itemMang->m_Item.m_Branch.m_bIsPreOrder = true;

		Helper::ItemHandleHelperI* pHelper = itemMang->m_ItemHandle.m_pHelper;

		uint32 c = 0;
		pHelper->onComplete(c);

		return form;
	});


	addButton("Install - Install Error", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_INSTALL, &pHelper);

		gcException e;
		pHelper->onError(e);

		return res;
	});




	addButton("Install - Download Tool", [this, setupActionWithStage](){
		return setupActionWithStage(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_DOWNLOADTOOL);
	});






	addButton("Install - Install Tool Other", [this, setupActionWithStage](){
		return setupActionWithStage(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_INSTALLTOOL);
	});

	addButton("Install - Install Tool Helper", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_INSTALLTOOL, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = 2;
		pHelper->onMcfProgress(info);

		return res;
	});

	addButton("Install - Install Tool Prog", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_INSTALLTOOL, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = 1;
		pHelper->onMcfProgress(info);

		return res;
	});




	addButton("Install - Install Check", [this, setupActionWithStage](){
		return setupActionWithStage(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_INSTALL_CHECK);
	});

	addButton("Install - Install Check Error", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_INSTALL_CHECK, &pHelper);

		gcException e;
		pHelper->onError(e);

		return res;
	});

	addButton("Install - Install Check Finished - Found", [this, setupActionWithStageAndHelper](){

		auto itemMang = new LangStubItemManager();
		auto form = new ItemForm(this);
		form->init(INSTALL_ACTION::IA_INSTALL, MCFBranch(), MCFBuild(), true, &itemMang->m_ItemHandle);
		
		auto stage = ITEM_STAGE::STAGE_INSTALL_CHECK;
		itemMang->m_ItemHandle.m_ChangeStageEvent(stage);
		form->Show();
		
		itemMang->m_Item.m_bIsInstalled = true;
		Helper::ItemHandleHelperI* pHelper = itemMang->m_ItemHandle.m_pHelper;

		uint32 c = 0;
		pHelper->onComplete(c);

		return form;
	});


	addButton("Install - Install Check Finished - Not Found", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_INSTALL_CHECK, &pHelper);

		uint32 c = 0;
		pHelper->onComplete(c);

		return res;
	});





	addButton("Install - Install Complex Init", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_INSTALL_COMPLEX, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = 1;
		pHelper->onMcfProgress(info);

		return res;
	});

	addButton("Install - Install Complex Install", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_INSTALL_COMPLEX, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = 0 + (2<<4);

		pHelper->onMcfProgress(info);

		return res;
	});

	addButton("Install - Install Complex Remove", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_INSTALL_COMPLEX, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = 0 + (0<<4);

		pHelper->onMcfProgress(info);

		return res;
	});

	addButton("Install - Install Complex Backup", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_INSTALL_COMPLEX, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = 0 + (1<<4);

		pHelper->onMcfProgress(info);

		return res;
	});




	addButton("Install - Validate", [this, setupActionWithStage](){
		return setupActionWithStage(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_VALIDATE);
	});

	addButton("Install - Validate Stage 1", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_VALIDATE, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = 1;
		info.hour = 1;
		info.min = 2;
		info.rate = 245;
		info.percent = 24;
		info.totalAmmount = 123;
		info.doneAmmount = 1;
		pHelper->onMcfProgress(info);

		return res;
	});

	addButton("Install - Validate Stage 2", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_VALIDATE, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = 2;
		info.hour = 1;
		info.min = 2;
		info.rate = 245;
		info.percent = 24;
		info.totalAmmount = 123;
		info.doneAmmount = 1;
		pHelper->onMcfProgress(info);

		return res;
	});

	addButton("Install - Validate Stage 3", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_VALIDATE, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = 3;
		info.hour = 1;
		info.min = 2;
		info.rate = 245;
		info.percent = 24;
		info.totalAmmount = 123;
		info.doneAmmount = 1;
		pHelper->onMcfProgress(info);

		return res;
	});

	addButton("Install - Validate Stage 4", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_VALIDATE, &pHelper);

		MCFCore::Misc::ProgressInfo info;
		info.flag = 4;
		info.hour = 1;
		info.min = 2;
		info.rate = 245;
		info.percent = 24;
		info.totalAmmount = 123;
		info.doneAmmount = 1;

		pHelper->onMcfProgress(info);

		return res;
	});

	addButton("Install - Validate Error", [this, setupActionWithStageAndHelper](){
		Helper::ItemHandleHelperI* pHelper = nullptr;
		auto res = setupActionWithStageAndHelper(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_VALIDATE, &pHelper);

		gcException e;
		pHelper->onError(e);

		return res;
	});


	//Need to setup task group.
	addButton("Install - Cancel/Wait", [this, setupActionWithStage](){
		return setupActionWithStage(INSTALL_ACTION::IA_INSTALL, ITEM_STAGE::STAGE_WAIT);
	});

	//ItemForm::verifyAfterHashFail
	//ItemForm::onShowError
	//ItemForm::onGatherInfoComplete
	//ItemForm::onShowToolPrompt
	//ItemForm::onShowPlatformError
	//ItemForm::onShowInstallPrompt
	//ItemForm::onShowComplexPrompt
	//ItemForm::onSelectBranch
	//ItemForm::launchError
	//ItemForm::stopStagePrompt
	//ItemForm::onShowWinLaunchDialog
	//ItemForm::showLaunchError
	//ItemForm::installTestMcf

	auto createTempFile = [](const char* szFileName)
	{
		try
		{
			if (UTIL::FS::isValidFile(szFileName))
				UTIL::FS::delFile(szFileName);

			UTIL::FS::FileHandle fh(szFileName, UTIL::FS::FILE_APPEND);
			fh.write("abcd", 4);
		}
		catch (...)
		{		
		}
	};

	addButton("Create Mcf - Select", [this](){

		auto itemMang = new LangStubItemManager();
		auto form = new CreateMCFForm(this, itemMang);

		form->setInfo(DesuraId());
		form->showInfo();

		form->Show();

		return form;
	});

	addButton("Create Mcf - Progress Init", [this, createTempFile](){

		auto itemMang = new LangStubItemManager();
		auto form = new CreateMCFForm(this, itemMang);

		createTempFile("CreateMCFProgress.mcf");

		form->setInfo(DesuraId());
		form->showProg("CreateMCFProgress.mcf");
		form->m_bPromptClose = false;

		form->Show();

		return form;
	});

	addButton("Create Mcf - Progress", [this, createTempFile](){

		auto itemMang = new LangStubItemManager();
		auto form = new CreateMCFForm(this, itemMang);

		createTempFile("CreateMCFProgress.mcf");

		form->setInfo(DesuraId());
		form->showProg("CreateMCFProgress.mcf");
		form->m_bPromptClose = false;

		auto pPage = dynamic_cast<CreateProgPage*>(form->m_pPage);

		MCFCore::Misc::ProgressInfo info;
		info.percent = 25;
		pPage->onProgress(info);

		form->Show();

		return form;
	});

	addButton("Create Mcf - Complete", [this, createTempFile](){

		auto itemMang = new LangStubItemManager();
		auto form = new CreateMCFForm(this, itemMang);

		createTempFile("CreateMCFComplete.mcf");

		form->setInfo(DesuraId());
		form->showOverView("CreateMCFComplete.mcf");

		form->Show();

		return form;
	});

	addButton("Create Mcf - Cancel", [this](){

		auto itemMang = new LangStubItemManager();
		auto form = new CreateMCFForm(this, itemMang);

		form->setInfo(DesuraId());
		form->Show();
		form->Close();

		return form;
	});

	addButton("Create Mcf - Error", [this, createTempFile](){

		auto itemMang = new LangStubItemManager();
		auto form = new CreateMCFForm(this, itemMang);

		createTempFile("CreateMCFError.mcf");

		form->setInfo(DesuraId());
		form->showProg("CreateMCFError.mcf");
		form->m_bPromptClose = false;

		auto pPage = dynamic_cast<CreateProgPage*>(form->m_pPage);

		MCFCore::Misc::ProgressInfo info;
		info.percent = 25;
		pPage->onProgress(info);

		form->Show();

		gcException e;
		pPage->onError(e);

		return form;
	});

	//upload mcf

	addButton("Upload Mcf - Select", [this](){

		auto itemMang = new LangStubItemManager();
		auto form = new UploadMCFForm(this, itemMang);

		form->setInfo(DesuraId());
		form->showInfo();

		form->Show();

		return form;
	});


	addButton("Upload Mcf - Select Path Error", [this](){

		auto itemMang = new LangStubItemManager();
		auto form = new UploadMCFForm(this, itemMang);

		form->setInfo_path(DesuraId(), "|");
		form->showInfo();

		form->Show();

		return form;
	});

	addButton("Upload Mcf - Resume", [this](){

		auto itemMang = new LangStubItemManager();
		auto form = new UploadMCFForm(this, itemMang);

		form->setInfo_key(DesuraId(), "|");
		form->showInfo();

		form->Show();

		return form;
	});

	addButton("Upload Mcf - Resume Complete Error", [this](){

		auto itemMang = new LangStubItemManager();
		auto form = new UploadMCFForm(this, itemMang);

		form->setInfo_key(DesuraId(), "|");
		form->showInfo();

		form->Show();

		auto pPage = dynamic_cast<UploadInfoPage*>(form->m_pPage);
		pPage->onResumeComplete(nullptr);

		return form;
	});

	addButton("Upload Mcf - File Validation Error", [this](){

		auto itemMang = new LangStubItemManager();
		auto form = new UploadMCFForm(this, itemMang);

		form->setInfo_key(DesuraId(), "|");
		form->showInfo();

		form->Show();

		auto pPage = dynamic_cast<UploadInfoPage*>(form->m_pPage);
		pPage->onFileValidate(false, nullptr);

		return form;
	});

	addButton("Upload Mcf - Progress Init", [this](){

		auto itemMang = new LangStubItemManager();
		auto form = new UploadMCFForm(this, itemMang);

		form->setInfo(DesuraId());
		form->showProg(0, 0);

		form->Show();

		return form;
	});

	addButton("Upload Mcf - Progress", [this](){

		auto itemMang = new LangStubItemManager();
		auto form = new UploadMCFForm(this, itemMang);

		form->setInfo(DesuraId());
		form->showProg(0, 0);

		auto pPage = dynamic_cast<UploadProgPage*>(form->m_pPage);

		UserCore::Misc::UploadInfo info;
		info.percent = 25;
		pPage->onProgress(info);

		form->Show();

		return form;
	});

	addButton("Upload Mcf - Progress Paused", [this](){

		auto itemMang = new LangStubItemManager();
		auto form = new UploadMCFForm(this, itemMang);

		form->setInfo(DesuraId());
		form->showProg(0, 0);

		auto pPage = dynamic_cast<UploadProgPage*>(form->m_pPage);

		UserCore::Misc::UploadInfo info;
		info.percent = 25;
		info.paused = true;
		pPage->onProgress(info);

		form->Show();

		return form;
	});

	addButton("Upload Mcf - Complete", [this](){

		auto itemMang = new LangStubItemManager();
		auto form = new UploadMCFForm(this, itemMang);

		form->setInfo(DesuraId());
		form->showProg(0, 0);

		auto pPage = dynamic_cast<UploadProgPage*>(form->m_pPage);

		uint32 status = 0;
		pPage->onComplete(status);

		form->Show();

		return form;
	});

	addButton("Upload Mcf - Error", [this](){

		auto itemMang = new LangStubItemManager();
		auto form = new UploadMCFForm(this, itemMang);

		form->setInfo(DesuraId());
		form->showProg(0, 0);

		auto pPage = dynamic_cast<UploadProgPage*>(form->m_pPage);

		UserCore::Misc::UploadInfo info;
		info.percent = 25;
		pPage->onProgress(info);

		form->Show();

		gcException e;
		pPage->onError(e);

		return form;
	});

	//browser issues
	addButton("No Browser", [this](){
		auto form = new LangBrowserWindow(this, "http://www.google.com", true);

		form->Show();
		return form;
	});

	//taskbar menu
	addButton("Task Bar Menu - offline", [this](){
		auto form = new TaskBarIcon(this);

		auto menu = form->CreatePopupMenu(true);
		PopupMenu(menu, 0, 0);

		return nullptr;
	});

	addButton("Task Bar Menu - Online", [this](){
		auto itemMang = new LangStubItemManager();
		auto form = new TaskBarIcon(this, itemMang);

		auto menu = form->CreatePopupMenu(false);
		PopupMenu(menu, 0, 0);

		return nullptr;
	});
	

	//Service Error
	addButton("Service Disconnect", [this](){
		auto form = new DesuraServiceError(this);
		form->Show();
		return form;
	});

	addButton("Service Disconnect - Error", [this](){
		auto form = new DesuraServiceError(this);
		form->Show();
		gcException e;
		form->onError(e);
		return form;
	});	

	addButton("Steam Warning", [this](){
		auto form = new SteamUserDialog(this);
		form->Show();
		return form;
	});

	//game disk req
	addButton("Game Disc", [this](){

		auto itemMang = new LangStubItemManager();
		auto form = new GameDiskForm(this, "", false);
		form->setInfo(DesuraId(), itemMang);
		form->Show();
		return form;
	});

	//Main form + controls
	addButton("Main Form", [this](){
		auto form = new MainForm(this, false, "");
		form->Show();
		return form;
	});

	//Settings page
	addButton("Settings Page", [this](){
		auto form = new LangBrowserWindow(this, GetGCThemeManager()->getWebPage("settings"));

		form->Show();
		return form;
	});

	//Play page
	addButton("Play Page - No Items", [this](){

		safe_delete(DesuraJSBinding::gs_pItemManager);
		auto form = new LangBrowserWindow(this, GetGCThemeManager()->getWebPage("playlist"));

		form->Show();
		return form;
	});
	
	addButton("Play Page", [this](){

		DesuraJSBinding::gs_pItemManager = new LangPlayItemManager();
		auto form = new LangBrowserWindow(this, GetGCThemeManager()->getWebPage("playlist"));

		form->Show();
		return form;
	});

	this->SetSizer( gSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

void disableChildrenButtons(wxWindow* window)
{
	for (auto child : window->GetChildren())
	{
		if (dynamic_cast<gcButton*>(child))
			child->Disable();

		disableChildrenButtons(child);
	}
}

void LanguageTestDialog::onButtonClicked(wxCommandEvent& event)
{
	if (event.GetId() == m_button1->GetId())
	{
		for (auto cb : m_ActionMap)
		{
			auto frame = cb.second();

			if (!frame)
				continue;

			m_vActiveFrames.push_back(frame);
			disableChildrenButtons(frame);
		}
	}
	else if (event.GetId() == m_button2->GetId())
	{
		for (auto win : m_vActiveFrames)
			win->Close(true);
	}

	else
	{
		auto it = m_ActionMap.find(event.GetId());

		if (it == m_ActionMap.end())
			return;

		auto frame = it->second();

		if (!frame)
			return;

		m_vActiveFrames.push_back(frame);
		disableChildrenButtons(frame);
	}
}

void LanguageTestDialog::onChoice(wxCommandEvent& event)
{
	auto strVal = m_choice1->GetStringSelection();
	
	GetLanguageManager().unloadAll();

	for (auto l : m_vLanguages)
	{
		if (strVal == l["name"])
		{
			gcString path = UTIL::OS::getDataPath();
			path += gcString("/language/{0}.xml", l["file"]);

			path = UTIL::FS::PathWithFile(path.c_str()).getFullPath();
			GetLanguageManager().loadFromFile(path.c_str());
			return;
		}
	}
}
