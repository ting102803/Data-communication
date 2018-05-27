

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
		// �� �������� �Ѱܹ��� ppayload�� �м��Ͽ� ChatDlg �������� �Ѱ��ش�.
		if(fapp_hdr->fapp_type == DATA_TYPE_BEGIN) // ������ ù �κ�
		{
			((CIPCAppDlg *)mp_aUpperLayer[0])->OnOffFileButton(FALSE);

			send_fileTotlen = fapp_hdr->fapp_totlen; // ���� ���� �������� ��ü ����
			hFile = CreateFile((char *)fapp_hdr->fapp_data,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				AfxMessageBox("���� ���� ����");
				return FALSE;
			}
			bMSGCheck = TRUE; // �����͸� �� �޾Ҵٰ� �޽����� ������ ���� ����ġ ������ BOOL �������� TRUE
			bGoLoop = FALSE;
			bFILE = FALSE;

			// ������
			AfxBeginThread(FileThread,this);
		}
		else if(fapp_hdr->fapp_type == DATA_TYPE_CONT) // ������ �߰� �κ�
		{
			receive_fileTotlen += fapp_hdr->fapp_totlen; // ���� ������ ���̸� ���� �ջ�

			GetBuff = (unsigned char*)malloc(fapp_hdr->fapp_totlen); // ���� ����
			memset(GetBuff,0,fapp_hdr->fapp_totlen); // ���� �ʱ�ȭ
			memcpy(GetBuff,fapp_hdr->fapp_data,fapp_hdr->fapp_totlen); // ���ۿ��ٰ� ���� data�� ��
			GetBuff[fapp_hdr->fapp_totlen] = '\0';

			bResult = WriteFile(hFile,GetBuff,fapp_hdr->fapp_totlen,&dwWrite,NULL); // ���۸� ���Ͽ� ��.

			progress_value = (100* receive_fileTotlen) / send_fileTotlen;
			((CIPCAppDlg *)mp_aUpperLayer[0])->m_ProgressCtrl.SetPos(progress_value);

			bMSGCheck = TRUE;
			bGoLoop = FALSE;
			bFILE = FALSE;
			
		}
		else if(fapp_hdr->fapp_type == DATA_TYPE_END) // ������ �� �κ�
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
				AfxMessageBox("���� ���� �Ϸ�"); // ���� �Ϸ� �޽��� â ���
			else
				AfxMessageBox("���� ���� ����"); // ���� ���� �޽��� â ���

			receive_fileTotlen = 0;	// ���� �Ϸ� �� ���� ���� ���� Ȯ���� ���� ���� ������ ���̸� �ʱ�ȭ
			((CIPCAppDlg *)mp_aUpperLayer[0])->m_ProgressCtrl.SetPos(0);
			((CIPCAppDlg *)mp_aUpperLayer[0])->OnOffFileButton(TRUE);
		}
	}
	else if(fapp_hdr->fapp_msg_type == MSG_TYPE_CHECK) // ���Ź��� ������ Ȯ�� �޽���
	{
		bGoLoop = TRUE;
	}
	else if(fapp_hdr->fapp_msg_type == MSG_TYPE_FAIL) // �߸��� ������ ���Ž� ������ �޽���
	{
		bMSGFail = TRUE;
	}

	return bSuccess ;
}

UINT CFileAppLayer::FileThread( LPVOID pParam )
{
	// ��� : ������ �о ���� (������ ����)

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
		// �ڵ鷯�� �������� ����
		hFile = CreateFile((char *)fapp_hdr->m_FilePath,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
		if(hFile == INVALID_HANDLE_VALUE) // �ڵ鷯�� ����� ��� �Ǿ����� Ȯ��
		{
			AfxMessageBox("�ùٸ� ���� ��ΰ� �ƴմϴ�.");
			((CIPCAppDlg *)fapp_hdr->mp_aUpperLayer[0])->OnOffFileButton(TRUE);
			return FALSE;
		}

		// ���ϰ�ο��� ���ϸ� ������
		fileName = fapp_hdr->m_FilePath;
		fileName = fileName.Right(fileName.GetLength()-fileName.ReverseFind('\\')-1);

		// ���� ��ü ũ��(byte)�� ������
		dwFileSize = GetFileSize(hFile,0);

		// ���� ��ü ũ�⸸ŭ ���� ���� ����
		pszBuf = (char *)malloc(dwFileSize);

		if(dwFileSize < FILE_READ_SIZE) // ���� ũ�Ⱑ �� ���� �о���̴� ũ�⺸�� ������ seq_num = 1
			tot_seq_num = 1;
		else // ���� ũ�Ⱑ �� ���� �о���̴� ũ�⺸�� ũ��, ���� ��ü ����� READ_SIZE�� ����.
			tot_seq_num = (dwFileSize/FILE_READ_SIZE)+1; // Sequential number �� ����
	}
	else
	{
		if(send_fileTotlen < FILE_READ_SIZE) // ���� ũ�Ⱑ �� ���� �о���̴� ũ�⺸�� ������ seq_num = 1
			tot_seq_num = 1;
		else // ���� ũ�Ⱑ �� ���� �о���̴� ũ�⺸�� ũ��, ���� ��ü ����� READ_SIZE�� ����.
			tot_seq_num = (send_fileTotlen/FILE_READ_SIZE)+1; // Sequential number �� ����
	}

	while(i<=tot_seq_num+1)//
	{
		if(bMSGCheck == TRUE)	// ���� �Ϸ� �޽��� ����
		{
			fapp_hdr->m_sHeader.fapp_totlen = 0;
			fapp_hdr->m_sHeader.fapp_msg_type = MSG_TYPE_CHECK;
			// TCP PORT ����
			((CTCPLayer*)(fapp_hdr->GetUnderLayer()))->SetDestinPort(TCP_PORT_FILE);

			bSuccess = fapp_hdr->mp_UnderLayer->Send((unsigned char*) &fapp_hdr->m_sHeader,FILE_HEADER_SIZE);

			bGoLoop = FALSE;
			bMSGCheck = FALSE;
		}
		if(bMSGFail == TRUE)	// ������ ��û �޽��� ����
		{
			bGoLoop = FALSE;
			bMSGFail = FALSE;
		}

		if(bGoLoop == TRUE)
		{
			if(i==0) //  ó���κ�
			{
				fapp_hdr->m_sHeader.fapp_type = DATA_TYPE_BEGIN;	// ����ȭ�� ������ 
				fapp_hdr->m_sHeader.fapp_msg_type = MSG_TYPE_FRAG; // �޽����� ����
				fapp_hdr->m_sHeader.fapp_totlen = dwFileSize; // ���� ��ü ũ��
				
				dwRead = fileName.GetLength(); // ���ϸ� ����
				memcpy(fapp_hdr->m_sHeader.fapp_data,fileName,dwRead); // ���ϸ� ����
				fapp_hdr->m_sHeader.fapp_data[dwRead] = '\0';
			}
			else if(i!=0 && i<=tot_seq_num) // �߰��κ�
			{
				// ������ ���������� �о��
				bResult = ReadFile(hFile,pszBuf,FILE_READ_SIZE,&dwRead,NULL);
								/* ReadFile(���� ����)
								* hFile :			File handler
								* pszBuf :			������ ������ �о���� ����
								* FILE_READ_SIZE :	�� �� �о���� ������ byte������ ��
								* dwRead :			�о���� ������ ������ �翡 ���� ��¿� �μ�
								* Overlapped ���� (���� NULL)
								*/
				dwState += dwRead;
				pszBuf[dwRead] = '\0';
				
				fapp_hdr->m_sHeader.fapp_totlen = dwRead; // �о�� ������ ũ��
				fapp_hdr->m_sHeader.fapp_type = DATA_TYPE_CONT;	// ����ȭ�� ������ Ÿ��
				fapp_hdr->m_sHeader.fapp_msg_type = MSG_TYPE_FRAG; // �޽����� ����
				
				fapp_hdr->m_sHeader.fapp_seq_num = i-1; // sequential number
				((CIPLayer*)(fapp_hdr->GetUnderLayer()->GetUnderLayer()))->SetFragOff(fapp_hdr->m_sHeader.fapp_seq_num);

				memcpy(fapp_hdr->m_sHeader.fapp_data,pszBuf,dwRead); // �о�� ������
				memset(pszBuf,0,dwRead);

				//  ���� �����Ȳ�� ���α׷����ٿ� ǥ������.
				progress_value = (100 * dwState) / dwFileSize ;
				((CIPCAppDlg *)fapp_hdr->mp_aUpperLayer[0])->m_ProgressCtrl.SetPos(progress_value);
			}
			else // ������ �κ�
			{
				CloseHandle(hFile); // ���� Handler�� �ݴ´�.
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
		AfxMessageBox("���� ���� �Ϸ�"); // ���ۿϷ� �޽��� â ���
	else
		AfxMessageBox("���� ���� ����"); // ���۽��� �޽��� â ���

	((CIPCAppDlg *)fapp_hdr->mp_aUpperLayer[0])->m_ProgressCtrl.SetPos(0);
	((CIPCAppDlg *)fapp_hdr->mp_aUpperLayer[0])->OnOffFileButton(TRUE);

	if(tot_seq_num!=1) // ������ ������ �ʾ��� ��쿡�� pszBuf�� �Ҵ� ���� �ʾ����Ƿ� free�� �� �ʿ䰡 ����
	{
		free(pszBuf);
	}

	bGoLoop = TRUE; // ���� �۽ſ��� �ݺ����� �����ϱ� ���ؼ� bGoLoop�� TRUE�� �ʱ�ȭ 

	return bSuccess ;
}