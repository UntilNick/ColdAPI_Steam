#pragma once
#include "../public SDK/ISteamRemoteStorage014.h"
#include "../Bridge.h"
#include "../ColdManager.h"

class SteamRemoteStorageIn014 : public ISteamRemoteStorage014
{
public:
	bool	FileWrite(const char* pchFile, const void* pvData, int32 cubData)
	{
		if (!Steam_Config::RemoteStorage)
			return true;
		if (cubData < NULL)
			return false;
		if (pvData <= NULL)
			return false;

		// Let's use std as more faster.
		std::FILE* File = std::fopen(ColdAPI_Storage::ConnectDirectoryToFile(pchFile), "wb");
		if (File)
		{
			std::fwrite(pvData, cubData, 1, File);
			std::fclose(File);
			return true;
		}
		return false;
	}
	int32	FileRead(const char* pchFile, void* pvData, int32 cubDataToRead)
	{
		if (!Steam_Config::RemoteStorage)
			return NULL;
		if (cubDataToRead < NULL)
			return NULL;
		if (pvData <= NULL)
			return NULL;

		// Let's use std as more faster.
		std::FILE* File = std::fopen(ColdAPI_Storage::ConnectDirectoryToFile(pchFile), "rb");
		if (File)
		{
			std::fseek(File, 0, SEEK_END);
			long FileSize = std::ftell(File);
			std::fseek(File, 0, SEEK_SET);

			// Let's check always if the read size is not bigger than the FileSize. We'll do it with min.
			int32_t Min = min(cubDataToRead, FileSize);
			std::fread(pvData, Min, 1, File);
			std::fclose(File);
			return Min;
		}
		return NULL;
	}


	SteamAPICall_t FileWriteAsync(const char* pchFile, const void* pvData, uint32 cubData)
	{
		return NULL;
	}


	SteamAPICall_t FileReadAsync(const char* pchFile, uint32 nOffset, uint32 cubToRead)
	{
		return NULL;
	}
	bool	FileReadAsyncComplete(SteamAPICall_t hReadCall, void* pvBuffer, uint32 cubToRead)
	{
		return true;
	}

	bool	FileForget(const char* pchFile)
	{
		if (!Steam_Config::RemoteStorage)
			return false;
		return true;
	}
	bool	FileDelete(const char* pchFile)
	{
		if (!Steam_Config::RemoteStorage)
			return false;
		const char* myfile = ColdAPI_Storage::ConnectDirectoryToFile(pchFile);
		if (GetFileAttributesA(myfile) == INVALID_FILE_ATTRIBUTES)
			return false;
		return (DeleteFileA(myfile) == TRUE);
	}

	SteamAPICall_t FileShare(const char* pchFile)
	{
		return NULL;
	}
	bool	SetSyncPlatforms(const char* pchFile, ERemoteStoragePlatform eRemoteStoragePlatform)
	{
		return true;
	}

	UGCFileWriteStreamHandle_t FileWriteStreamOpen(const char* pchFile)
	{
		return NULL;
	}
	bool FileWriteStreamWriteChunk(UGCFileWriteStreamHandle_t writeHandle, const void* pvData, int32 cubData)
	{
		return true;
	}
	bool FileWriteStreamClose(UGCFileWriteStreamHandle_t writeHandle)
	{
		return true;
	}
	bool FileWriteStreamCancel(UGCFileWriteStreamHandle_t writeHandle)
	{
		return true;
	}

	bool	FileExists(const char* pchFile)
	{
		if (!Steam_Config::RemoteStorage)
			return false;
		return (GetFileAttributesA(ColdAPI_Storage::ConnectDirectoryToFile(pchFile)) != INVALID_FILE_ATTRIBUTES);
	}
	bool	FilePersisted(const char* pchFile)
	{
		if (!Steam_Config::RemoteStorage)
			return false;
		return true;
	}
	int32	GetFileSize(const char* pchFile)
	{
		if (!Steam_Config::RemoteStorage)
			return NULL;
		const char* myfile = ColdAPI_Storage::ConnectDirectoryToFile(pchFile);
		// Let's use std as more faster.
		std::FILE* File = std::fopen(myfile, "rb");
		if (File)
		{
			std::fseek(File, 0, SEEK_END);
			long FileSize = std::ftell(File);
			std::fseek(File, 0, SEEK_SET);
			std::fclose(File);
			return FileSize;
		}
		return NULL;
	}
	int64	GetFileTimestamp(const char* pchFile)
	{
		return time(NULL) - 3000;
	}
	ERemoteStoragePlatform GetSyncPlatforms(const char* pchFile)
	{
		return k_ERemoteStoragePlatformAll;
	}

	int32 GetFileCount()
	{
		if (!Steam_Config::RemoteStorage)
			return NULL;
		ColdAPI_Storage::FillFileStructure(ColdAPI_Storage::GetStorageDirectory());
		return FilesMatrix.size(); // Return the vector size
	}
	const char* GetFileNameAndSize(int iFile, int32* pnFileSizeInBytes)
	{
		if (!Steam_Config::RemoteStorage)
			return "";

		ColdAPI_Storage::FillFileStructure(ColdAPI_Storage::GetStorageDirectory());

		if (iFile <= FilesMatrix.size())
		{
			std::string FileName = FilesMatrix.at(iFile);

			// Let's use std as more faster.
			std::FILE* File = std::fopen(ColdAPI_Storage::ConnectDirectoryToFile(FileName.c_str()), "rb");
			if (File)
			{
				std::fseek(File, 0, SEEK_END);
				long FileSize = std::ftell(File);
				std::fseek(File, 0, SEEK_SET);
				std::fclose(File);
				if (pnFileSizeInBytes != NULL && pnFileSizeInBytes > NULL)
					*pnFileSizeInBytes = FileSize;
				return FileName.c_str();
			}
		}
		return "";
	}

	bool GetQuota(uint64* pnTotalBytes, uint64* puAvailableBytes)
	{
		if (pnTotalBytes == NULL || pnTotalBytes < NULL)
			return false;
		if (puAvailableBytes == NULL || puAvailableBytes < NULL)
			return false;
		*pnTotalBytes = NULL;
		*puAvailableBytes = INT_MAX;
		return true;
	}
	bool IsCloudEnabledForAccount()
	{
		return true;
	}
	bool IsCloudEnabledForApp()
	{
		return true;
	}
	void SetCloudEnabledForApp(bool bEnabled)
	{
		return;
	}

	SteamAPICall_t UGCDownload(UGCHandle_t hContent, uint32 unPriority)
	{
		if (!Steam_Config::RemoteStorage)
			return NULL;

		ColdAPI_Storage::FillFileStructure(ColdAPI_Storage::GetUGCDirectory());

		if (FilesMatrix.size() >= hContent)
		{
			// Read the UGC File.
			std::string FileName = FilesMatrix.at(hContent);
			std::FILE* File = std::fopen(ColdAPI_Storage::ConnectUGCDirectoryToFile(FileName.c_str()), "rb");
			if (File)
			{
				std::fseek(File, 0, SEEK_END);
				long FileSize = std::ftell(File);
				std::fseek(File, 0, SEEK_SET);
				std::fclose(File);

				auto Response = new RemoteStorageDownloadUGCResult_t();
				auto RequestID = SteamCallback::RegisterCall(true);
				Response->m_eResult = k_EResultOK;
				Response->m_hFile = hContent;
				Response->m_nAppID = Steam_Config::AppId;
				Response->m_nSizeInBytes = FileSize;
				std::memcpy(Response->m_pchFileName, FileName.c_str(), MAX_PATH);
				Response->m_ulSteamIDOwner = Steam_Config::UserID;
				SteamCallback::CreateNewRequest(Response, sizeof(*Response), Response->k_iCallback, RequestID);
				return RequestID;
			}
		}
		return NULL;
	}

	bool	GetUGCDownloadProgress(UGCHandle_t hContent, int32* pnBytesDownloaded, int32* pnBytesExpected)
	{
		if (pnBytesDownloaded == NULL || pnBytesDownloaded < NULL)
			return false;
		if (pnBytesExpected == NULL || pnBytesExpected < NULL)
			return false;
		*pnBytesDownloaded = 10;
		*pnBytesExpected = 10;
		return true;
	}

	bool	GetUGCDetails(UGCHandle_t hContent, AppId_t* pnAppID, char** ppchName, int32* pnFileSizeInBytes, CSteamID* pSteamIDOwner)
	{
		if (!Steam_Config::RemoteStorage)
			return false;

		ColdAPI_Storage::FillFileStructure(ColdAPI_Storage::GetUGCDirectory());

		if (FilesMatrix.size() >= hContent)
		{
			// Read the UGC File.
			std::string FileName = FilesMatrix.at(hContent);
			std::FILE* File = std::fopen(ColdAPI_Storage::ConnectUGCDirectoryToFile(FileName.c_str()), "rb");
			if (File)
			{
				std::fseek(File, 0, SEEK_END);
				long FileSize = std::ftell(File);
				std::fseek(File, 0, SEEK_SET);
				std::fclose(File);
				if (pnAppID != NULL && pnAppID > NULL)
					*pnAppID = Steam_Config::AppId;
				if (ppchName != NULL && ppchName > NULL) {
					*ppchName = (char*)std::malloc(std::strlen(FileName.c_str()) + 10);
					std::strcpy(*ppchName, FileName.c_str());
				}
				if (pnFileSizeInBytes != NULL && pnFileSizeInBytes > NULL)
					*pnFileSizeInBytes = FileSize;
				if (pSteamIDOwner != NULL && pSteamIDOwner > NULL)
					*pSteamIDOwner = Steam_Config::UserID;
				return true;
			}
		}
		return false;
	}

	int32	UGCRead(UGCHandle_t hContent, void* pvData, int32 cubDataToRead, uint32 cOffset, EUGCReadAction eAction)
	{
		if (!Steam_Config::RemoteStorage)
			return NULL;
		if (cubDataToRead < NULL)
			return NULL;
		if (pvData <= NULL)
			return NULL;

		ColdAPI_Storage::FillFileStructure(ColdAPI_Storage::GetUGCDirectory());

		if (FilesMatrix.size() >= hContent)
		{
			// Read the UGC File.
			std::string FileName = FilesMatrix.at(hContent);
			std::FILE* File = std::fopen(ColdAPI_Storage::ConnectUGCDirectoryToFile(FileName.c_str()), "rb");
			if (File)
			{
				std::fseek(File, 0, SEEK_END);
				long FileSize = std::ftell(File);
				std::fseek(File, 0, SEEK_SET);

				// Let's check always if the read size is not bigger than the FileSize. We'll do it with min.
				int32_t Min = min(cubDataToRead, FileSize);
				std::fread(pvData, Min, 1, File);
				std::fclose(File);
				return Min;
			}
		}
		return NULL;
	}

	int32	GetCachedUGCCount()
	{
		return NULL;
	}
	UGCHandle_t GetCachedUGCHandle(int32 iCachedContent)
	{
		return NULL;
	}

#if defined(_PS3) || defined(_SERVER)
	
	void GetFileListFromServer()
	{
		return;
	}
	bool FileFetch(const char* pchFile)
	{
		return true;
	}
	bool FilePersist(const char* pchFile)
	{
		return true;
	}
	bool SynchronizeToClient()
	{
		return true;
	}
	bool SynchronizeToServer()
	{
		return true;
	}
	bool ResetFileRequestState()
	{
		return true;
	}
#endif

	SteamAPICall_t	PublishWorkshopFile(const char* pchFile, const char* pchPreviewFile, AppId_t nConsumerAppId, const char* pchTitle, const char* pchDescription, ERemoteStoragePublishedFileVisibility eVisibility, SteamParamStringArray_t* pTags, EWorkshopFileType eWorkshopFileType)
	{
		return NULL;
	}
	PublishedFileUpdateHandle_t CreatePublishedFileUpdateRequest(PublishedFileId_t unPublishedFileId)
	{
		return NULL;
	}
	bool UpdatePublishedFileFile(PublishedFileUpdateHandle_t updateHandle, const char* pchFile)
	{
		return false;
	}
	bool UpdatePublishedFilePreviewFile(PublishedFileUpdateHandle_t updateHandle, const char* pchPreviewFile)
	{
		return false;
	}
	bool UpdatePublishedFileTitle(PublishedFileUpdateHandle_t updateHandle, const char* pchTitle)
	{
		return false;
	}
	bool UpdatePublishedFileDescription(PublishedFileUpdateHandle_t updateHandle, const char* pchDescription)
	{
		return false;
	}
	bool UpdatePublishedFileVisibility(PublishedFileUpdateHandle_t updateHandle, ERemoteStoragePublishedFileVisibility eVisibility)
	{
		return false;
	}
	bool UpdatePublishedFileTags(PublishedFileUpdateHandle_t updateHandle, SteamParamStringArray_t* pTags)
	{
		return false;
	}

	SteamAPICall_t	CommitPublishedFileUpdate(PublishedFileUpdateHandle_t updateHandle)
	{
		return NULL;
	}
	SteamAPICall_t	GetPublishedFileDetails(PublishedFileId_t unPublishedFileId, uint32 unMaxSecondsOld)
	{
		return NULL;
	}

	SteamAPICall_t	DeletePublishedFile(PublishedFileId_t unPublishedFileId)
	{
		return NULL;
	}
	SteamAPICall_t	EnumerateUserPublishedFiles(uint32 unStartIndex)
	{
		return NULL;
	}

	SteamAPICall_t	SubscribePublishedFile(PublishedFileId_t unPublishedFileId)
	{
		return NULL;
	}

	SteamAPICall_t	EnumerateUserSubscribedFiles(uint32 unStartIndex)
	{
		return NULL;
	}

	SteamAPICall_t	UnsubscribePublishedFile(PublishedFileId_t unPublishedFileId)
	{
		return NULL;
	}
	bool UpdatePublishedFileSetChangeDescription(PublishedFileUpdateHandle_t updateHandle, const char* pchChangeDescription)
	{
		return false;
	}

	SteamAPICall_t	GetPublishedItemVoteDetails(PublishedFileId_t unPublishedFileId)
	{
		return NULL;
	}

	SteamAPICall_t	UpdateUserPublishedItemVote(PublishedFileId_t unPublishedFileId, bool bVoteUp)
	{
		return NULL;
	}

	SteamAPICall_t	GetUserPublishedItemVoteDetails(PublishedFileId_t unPublishedFileId)
	{
		return NULL;
	}

	SteamAPICall_t	EnumerateUserSharedWorkshopFiles(CSteamID steamId, uint32 unStartIndex, SteamParamStringArray_t* pRequiredTags, SteamParamStringArray_t* pExcludedTags)
	{
		return NULL;
	}

	SteamAPICall_t	PublishVideo(EWorkshopVideoProvider eVideoProvider, const char* pchVideoAccount, const char* pchVideoIdentifier, const char* pchPreviewFile, AppId_t nConsumerAppId, const char* pchTitle, const char* pchDescription, ERemoteStoragePublishedFileVisibility eVisibility, SteamParamStringArray_t* pTags)
	{
		return NULL;
	}

	SteamAPICall_t	SetUserPublishedFileAction(PublishedFileId_t unPublishedFileId, EWorkshopFileAction eAction)
	{
		return NULL;
	}

	SteamAPICall_t	EnumeratePublishedFilesByUserAction(EWorkshopFileAction eAction, uint32 unStartIndex)
	{
		return NULL;
	}
	SteamAPICall_t	EnumeratePublishedWorkshopFiles(EWorkshopEnumerationType eEnumerationType, uint32 unStartIndex, uint32 unCount, uint32 unDays, SteamParamStringArray_t* pTags, SteamParamStringArray_t* pUserTags)
	{
		return NULL;
	}


	SteamAPICall_t UGCDownloadToLocation(UGCHandle_t hContent, const char* pchLocation, uint32 unPriority)
	{
		return NULL;
	}

	private:
		void* FileBuffer = 0;
};