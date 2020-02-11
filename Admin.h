#include"User.h"

class Admin:public User
{
public:
	void Menu(DALTransaction& trans);
	void AddUser(DALTransaction& trans);
	void DeleteUser(DALTransaction& trans);
	void ModifyUserRole(DALTransaction& trans);
};