//Message.h
#ifndef MESSAGE_H
#define MESSAGE_H

#define TABLESIZE 256
#define _size 1024*1024

class Message
{
	public:
		Message();
		
		void encryptMessage();
		void decryptMessage();
		
		const TCHAR* getUnMessage() const;
		const TCHAR* getEnMessage() const;
		
		//IMPORTANT!! -- always set message first
		void setMessage(const TCHAR * pMsg);
		//IMPORTANT!! -- always set message first
		void setKey(const TCHAR * pTemp);
		
		~Message();
		
	private:
		TCHAR* pUnMessage;
		TCHAR* pEnMessage;
		TCHAR* pKey;
		TCHAR  table[TABLESIZE][TABLESIZE];

	private:
		void initTable();

};

#endif