

#include "stdafx.h"
#include "ipc.h"
#include "FileAppLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CFileAppLayer::CFileAppLayer( char* pName )
	: CBaseLayer( pName )
{
	bGoLoop = TRUE;
	receive_fileTotlen = 0;
	ResetHeader( );
}

CFileAppLayer::~CFileAppLayer()
{
}

void CFileAppLayer::ResetHeader()
{
	m_sHeader.fapp_totlen  = 0x00000000 ;
	m_sHeader.fapp_type    = 0x0000 ;
	m_sHeader.fapp_msg_type = 0x00 ;
	m_sHeader.unused = 0x00;
	m_sHeader.fapp_seq_num = 0x00000000 ;

	memset( m_sHeader.fapp_data, 0, APP_DATA_SIZE) ;
}

BOOL CFileAppLayer::Send(unsigned char* filePath)
{
	m_FilePath = filePath;
	bFILE = TRUE;

	((CIPCAppDlg *)mp_aUpperLayer[0])->OnOffFileButton(FALSE);

	AfxBeginThread(FileThread,this);
	return TRUE;
}

BOOL CFileAppLayer::Receive(unsigned char* ppayload)
{
	static HANDLE hFile = NULL;
	DWORD dwWrite=0, dwState=0;
	
	int progress_value;

	BOOL bResult;
	BOOL bSuccess = FALSE;
	
	LPFILE_APP fapp_hdr = (LPFILE_APP) ppayload ;
	unsigned char *GetBuff; 
	
	if(hFile == INVALID_HANDLE_VALUE) 
	{
		return FALSE;
	}
	if(fapp_hdr->fapp_msg_type == MSG_TYPE_FRAG) 
	{
		// 밑 계층에서 넘겨받은 ppayload를 분석하여 ChatDlg 계층으로 넘겨준다.
		if(fapp_hdr->fapp_type == DATA_TYPE_BEGIN) // 데이터 첫 부분
		{
			((CIPCAppDlg *)mp_aUpperLayer[0])->OnOffFileButton(FALSE);

			send_fileTotlen = fapp_hdr->fapp_totlen; // 수신 받을 데이터의 전체 길이
			hFile = CreateFile((char *)fapp_hdr->fapp_data,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				AfxMessageBox("파일 생성 오류");
				return FALSE;
			}
			bMSGCheck = TRUE; // 데이터를 잘 받았다고 메시지를 보내기 위한 스위치 역할의 BOOL 변수값을 TRUE
			bGoLoop = FALSE;
			bFILE = FALSE;

			// 쓰레드
			AfxBeginThread(FileThread,this);
		}
		else if(fapp_hdr->fapp_type == DATA_TYPE_CONT) // 데이터 중간 부분
		{
			receive_fileTotlen += fapp_hdr->fapp_totlen; // 받은 파일의 길이를 누적 합산

			GetBuff = (unsigned char*)malloc(fapp_hdr->fapp_totlen); // 버퍼 생성
			memset(GetBuff,0,fapp_hdr->fapp_totlen); // 버퍼 초기화
			memcpy(GetBuff,fapp_hdr->fapp_data,fapp_hdr->fapp_totlen); // 버퍼에다가 받은 data를 씀
			GetBuff[fapp_hdr->fapp_totlen] = '\0';

			bResult = WriteFile(hFile,GetBuff,fapp_hdr->fapp_totlen,&dwWrite,NULL); // 버퍼를 파일에 씀.

			progress_value = (100* receive_fileTotlen) / send_fileTotlen;
			((CIPCAppDlg *)mp_aUpperLayer[0])->m_ProgressCtrl.SetPos(progress_value);

			bMSGCheck = TRUE;
			bGoLoop = FALSE;
			bFILE = FALSE;
			
		}
		else if(fapp_hdr->fapp_type == DATA_TYPE_END) // 데이터 끝 부분
		{
			if(send_fileTotlen == receive_fileTotlen)
				bSuccess = TRUE;
			else
				bSuccess = FALSE;

			CloseHandle(hFile);
			bMSGCheck = FALSE;
			bGoLoop = FALSE;
			bFILE = FALSE;
			

			if(bSuccess==TRUE)
				AfxMessageBox("파일 수신 완료"); // 수신 완료 메시지 창 출력
			else
				AfxMessageBox("파일 수신 실패"); // 수신 실패 메시지 창 출력

			receive_fileTotlen = 0;	// 수신 완료 후 다음 파일 수신 확인을 위해 받은 파일의 길이를 초기화
			((CIPCAppDlg *)mp_aUpperLayer[0])->m_ProgressCtrl.SetPos(0);
			((CIPCAppDlg *)mp_aUpperLayer[0])->OnOffFileButton(TRUE);
		}
	}
	else if(fapp_hdr->fapp_msg_type == MSG_TYPE_CHECK) // 수신받은 데이터 확인 메시지
	{
		bGoLoop = TRUE;
	}
	else if(fapp_hdr->fapp_msg_type == MSG_TYPE_FAIL) // 잘못된 데이터 수신시 보내는 메시지
	{
		bMSGFail = TRUE;
	}

	return bSuccess ;
}

UINT CFileAppLayer::FileThread( LPVOID pParam )
{
	// 기능 : 파일을 읽어서 전송 (쓰레드 동작)

	CFileAppLayer *fapp_hdr = (CFileAppLayer *)pParam;
	BOOL bSuccess = FALSE ;
	
	HANDLE hFile;
	DWORD dwRead=0, dwFileSize, dwState=0;
	char *pszBuf;
	BOOL bResult;
	
	int progress_value;
	int i=0, tot_seq_num;
	CString fileName;
	
	if(bFILE==TRUE)
	{
		// 핸들러에 파일정보 저장
		hFile = CreateFile((char *)fapp_hdr->m_FilePath,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
		if(hFile == INVALID_HANDLE_VALUE) // 핸들러에 제대로 등록 되었는지 확인
		{
			AfxMessageBox("올바른 파일 경로가 아닙니다.");
			((CIPCAppDlg *)fapp_hdr->mp_aUpperLayer[0])->OnOffFileButton(TRUE);
			return FALSE;
		}

		// 파일경로에서 파일명만 가져옴
		fileName = fapp_hdr->m_FilePath;
		fileName = fileName.Right(fileName.GetLength()-fileName.ReverseFind('\\')-1);

		// 파일 전체 크기(byte)를 가져옴
		dwFileSize = GetFileSize(hFile,0);

		// 파일 전체 크기만큼 버퍼 공간 생성
		pszBuf = (char *)malloc(dwFileSize);

		if(dwFileSize < FILE_READ_SIZE) // 파일 크기가 한 번에 읽어들이는 크기보다 작으면 seq_num = 1
			tot_seq_num = 1;
		else // 파일 크기가 한 번에 읽어들이는 크기보다 크면, 파일 전체 사이즈를 READ_SIZE로 나눔.
			tot_seq_num = (dwFileSize/FILE_READ_SIZE)+1; // Sequential number 총 개수
	}
	else
	{
		if(send_fileTotlen < FILE_READ_SIZE) // 파일 크기가 한 번에 읽어들이는 크기보다 작으면 seq_num = 1
			tot_seq_num = 1;
		else // 파일 크기가 한 번에 읽어들이는 크기보다 크면, 파일 전체 사이즈를 READ_SIZE로 나눔.
			tot_seq_num = (send_fileTotlen/FILE_READ_SIZE)+1; // Sequential number 총 개수
	}

	while(i<=tot_seq_num+1)//
	{
		if(bMSGCheck == TRUE)	// 수신 완료 메시지 전송
		{
			fapp_hdr->m_sHeader.fapp_totlen = 0;
			fapp_hdr->m_sHeader.fapp_msg_type = MSG_TYPE_CHECK;
			// TCP PORT 설정
			((CTCPLayer*)(fapp_hdr->GetUnderLayer()))->SetDestinPort(TCP_PORT_FILE);

			bSuccess = fapp_hdr->mp_UnderLayer->Send((unsigned char*) &fapp_hdr->m_sHeader,FILE_HEADER_SIZE);

			bGoLoop = FALSE;
			bMSGCheck = FALSE;
		}
		if(bMSGFail == TRUE)	// 재전송 요청 메시지 전송
		{
			bGoLoop = FALSE;
			bMSGFail = FALSE;
		}

		if(bGoLoop == TRUE)
		{
			if(i==0) //  처음부분
			{
				fapp_hdr->m_sHeader.fapp_type = DATA_TYPE_BEGIN;	// 단편화된 데이터 
				fapp_hdr->m_sHeader.fapp_msg_type = MSG_TYPE_FRAG; // 메시지의 종류
				fapp_hdr->m_sHeader.fapp_totlen = dwFileSize; // 파일 전체 크기
				
				dwRead = fileName.GetLength(); // 파일명 길이
				memcpy(fapp_hdr->m_sHeader.fapp_data,fileName,dwRead); // 파일명 저장
				fapp_hdr->m_sHeader.fapp_data[dwRead] = '\0';
			}
			else if(i!=0 && i<=tot_seq_num) // 중간부분
			{
				// 파일을 순차적으로 읽어옴
				bResult = ReadFile(hFile,pszBuf,FILE_READ_SIZE,&dwRead,NULL);
								/* ReadFile(인자 순서)
								* hFile :			File handler
								* pszBuf :			파일의 내용을 읽어들일 버퍼
								* FILE_READ_SIZE :	한 번 읽어들일 파일의 byte단위의 수
								* dwRead :			읽어들인 파일의 데이터 양에 대한 출력용 인수
								* Overlapped 설정 (보통 NULL)
								*/
				dwState += dwRead;
				pszBuf[dwRead] = '\0';
				
				fapp_hdr->m_sHeader.fapp_totlen = dwRead; // 읽어온 데이터 크기
				fapp_hdr->m_sHeader.fapp_type = DATA_TYPE_CONT;	// 단편화된 데이터 타입
				fapp_hdr->m_sHeader.fapp_msg_type = MSG_TYPE_FRAG; // 메시지의 종류
				
				fapp_hdr->m_sHeader.fapp_seq_num = i-1; // sequential number
				((CIPLayer*)(fapp_hdr->GetUnderLayer()->GetUnderLayer()))->SetFragOff(fapp_hdr->m_sHeader.fapp_seq_num);

				memcpy(fapp_hdr->m_sHeader.fapp_data,pszBuf,dwRead); // 읽어온 데이터
				memset(pszBuf,0,dwRead);

				//  현재 진행상황을 프로그래스바에 표시해줌.
				progress_value = (100 * dwState) / dwFileSize ;
				((CIPCAppDlg *)fapp_hdr->mp_aUpperLayer[0])->m_ProgressCtrl.SetPos(progress_value);
			}
			else // 마지막 부분
			{
				CloseHandle(hFile); // 파일 Handler를 닫는다.
				fapp_hdr->m_sHeader.fapp_totlen = 0;
				fapp_hdr->m_sHeader.fapp_msg_type = MSG_TYPE_FRAG;
				fapp_hdr->m_sHeader.fapp_type = DATA_TYPE_END;
				memset(fapp_hdr->m_sHeader.fapp_data,0,APP_DATA_SIZE);
				dwRead = 0;
			}

			//bGoLoop = FALSE;

			bSuccess = fapp_hdr->mp_UnderLayer->Send((unsigned char*) &fapp_hdr->m_sHeader,FILE_HEADER_SIZE+dwRead);
			memset(fapp_hdr->m_sHeader.fapp_data,0,dwRead);
			i++;
		}
	}
	
	if(bSuccess==TRUE)
		AfxMessageBox("파일 전송 완료"); // 전송완료 메시지 창 출력
	else
		AfxMessageBox("파일 전송 실패"); // 전송실패 메시지 창 출력

	((CIPCAppDlg *)fapp_hdr->mp_aUpperLayer[0])->m_ProgressCtrl.SetPos(0);
	((CIPCAppDlg *)fapp_hdr->mp_aUpperLayer[0])->OnOffFileButton(TRUE);

	if(tot_seq_num!=1) // 파일을 나누지 않았을 경우에는 pszBuf가 할당 되지 않았으므로 free를 할 필요가 없음
	{
		free(pszBuf);
	}

	bGoLoop = TRUE; // 다음 송신에서 반복문에 진입하기 위해서 bGoLoop를 TRUE로 초기화 

	return bSuccess ;
}