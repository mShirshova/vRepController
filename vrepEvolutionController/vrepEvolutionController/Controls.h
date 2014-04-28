#ifndef _CONTROLS_H_INCLUDED
#define _CONTROLS_H_INCLUDED

#include <windows.h>
#include "resource.h"
#include <vector>
using namespace std;

class SimpleControl
{
public:
    SimpleControl (HWND hwndParent, int id, BOOL initialState=TRUE)
        : _hWnd (GetDlgItem (hwndParent, id)), _id(id)
    {
        if (initialState == FALSE)
         disable();
   }
   void hide ()
   {
      ::ShowWindow(_hWnd , SW_HIDE);
   }
   void show ()
   {
      ::ShowWindow(_hWnd , SW_SHOW);
   }
   BOOL isVisible()
   {
      return(::IsWindowVisible(_hWnd ));
   }
   void setFocus ()
   {
       ::SetFocus (_hWnd);
   }
   void enable()
   {
       ::EnableWindow(_hWnd , TRUE);
   }
   void disable()
   {
       ::EnableWindow(_hWnd , FALSE);
   }
	HWND hwnd () const { return _hWnd; }

protected:
    HWND _hWnd;
	int _id;
};

class Button : public SimpleControl
{
public:
   Button(HWND hwndParent, int id, BOOL initialState=TRUE)
       : SimpleControl(hwndParent, id, initialState)
   {}
   void setName( char const * newName )
   {
      SendMessage(_hWnd, WM_SETTEXT, 0, (LPARAM)newName );
   }
};

class CheckBox : public Button
{
public:
   CheckBox (HWND hwndParent, int id, BOOL initialState=TRUE)
       : Button(hwndParent, id, initialState)
   {}
   BOOL isChecked()
   {
      return( SendMessage(_hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED );
   }
   void check()
   {
      SendMessage( _hWnd, BM_SETCHECK, (WPARAM) BST_CHECKED, 0);
   }
   void unCheck()
   {
      SendMessage( _hWnd, BM_SETCHECK, (WPARAM) BST_UNCHECKED, 0);
   }
};

class RadioButton: public Button
{
public:
   RadioButton (HWND hwndParent, int id, BOOL initialState=TRUE)
       : Button(hwndParent, id, initialState)
   {}
   BOOL isSelected()
   {
      return( SendMessage( _hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED );
   }
   void select()
   {
      SendMessage( _hWnd, BM_SETCHECK, (WPARAM) BST_CHECKED, 0);
   }
};

class Edit: public SimpleControl
{
public:
   Edit (HWND hwndParent, int id, BOOL initialState=TRUE)
        :	SimpleControl (hwndParent, id, initialState)
   {}

   void setString (char* buf)
   {
       SendMessage (_hWnd, WM_SETTEXT, 0, (LPARAM) buf);
	   DWORD err = GetLastError();
   }

   void addMessageS(string s, bool shifDown = true)
   {
	   addMessage(s.data(), shifDown);
   }

   void addMessage(const char* buf, bool shifDown = true)
   {
	   int ndx = GetWindowTextLength(_hWnd);
	   SendMessage (_hWnd, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
	   if(shifDown)
		   SendMessage (_hWnd, EM_REPLACESEL, 0, (LPARAM) ((LPSTR)"\r\n>>> ")); 
	   else
		   SendMessage (_hWnd, EM_REPLACESEL, 0, (LPARAM) ((LPSTR)">>> ")); 

	   ndx = GetWindowTextLength(_hWnd);
	   SendMessage (_hWnd, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
	   SendMessage (_hWnd, EM_REPLACESEL, 0, (LPARAM)buf); 
   }

   // code is the HIWORD (wParam)
   static BOOL isChanged (int code)
   {
       return code == EN_CHANGE;
   }

   int getLength ()
   {
       return (int)(SendMessage (_hWnd, WM_GETTEXTLENGTH, 0, 0));
   }

   void getString (char* buf, int len)
   {
       SendMessage (_hWnd, WM_GETTEXT, (WPARAM) len, (LPARAM) buf);
   }

   int getInt()
   {
	   char buff[30];
	   getString(buff,30);
	   return atoi(buff);
   }

   void select ()
   {
       SendMessage (_hWnd, EM_SETSEL, 0, -1);
   }

   void clearSelection ()
   {
       SendMessage (_hWnd, EM_SETSEL, -1, 0);
   }
};

class MComboBox: public SimpleControl
{
public:
   MComboBox (HWND hwndParent, int id, BOOL initialState=TRUE)
        :	SimpleControl (hwndParent, id, initialState)
   {}

   void setItems(vector<string>& items)
   {
	   _items = items;

	   for(int i=0; i<_items.size(); ++i)
	   {
		   SendMessage(_hWnd,(UINT) CB_ADDSTRING,(WPARAM)0,(LPARAM)_items[i].data()); 
	   }
   }

   void clear()
   {
	   for(int i=0; i<_items.size(); ++i)
	   {
		   SendMessage(_hWnd,(UINT) CB_DELETESTRING,(WPARAM)0,(LPARAM)i); 
	   }
	   _items.resize(0);
   }

   int getCurSelection()
   {
	  return SendMessage(_hWnd, (UINT) CB_GETCURSEL, 
                (WPARAM) 0, (LPARAM) 0);
   }

   void setCurSelection(int index)
   {
	   if(index >= 0 && index < _items.size())
	   {
		   SendMessage(_hWnd, CB_SETCURSEL, (WPARAM)index, (LPARAM)0);
	   }
   }

   vector<string>& getItems()
   {
	   return _items;
   }

   protected:
    vector<string> _items;
};


#endif // _CONTROLS_H_INCLUDED
