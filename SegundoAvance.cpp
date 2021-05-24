#include <windows.h>
#include <CommCtrl.h>
#include <shellapi.h>
#include <mmsystem.h>
#include <fstream>
#include <string>
#include "resource.h"

using namespace std;
int GLOBAL_IDUSUARIO = 1;
int GLOBAL_IDPRODUCTO = 1;
HINSTANCE handlerglobal;
HWND hLBProducto;
HWND hLBProductoSeleccionado;
HWND hLBProductoSeleccionado1;
HWND hLBProductoSeleccionado2;
HWND hLBProductoSeleccionado3;
HWND hLBProductoSeleccionado4;
HWND hLBProductoSeleccionado5;

fstream archivo;
bool exitiniciarsesion = false;
bool imagen1cargadaProducto = false;
bool imagen2cargadaProducto = false;

struct Usuario {
	int IDUsuario;
	string username;
	string password;
	Usuario* previo;
	Usuario* siguiente;
} *oUsuario, *aUsuario, *iniciarsesion;

/*struct InformacionVendedor {
	int IDUsuario;
	int IDVendedor;
	string nombreVendedor;
	string atlasVendedor;
	InformacionVendedor* previo;
	InformacionVendedor* siguiente;
} *oInformacionVendedor, *aInformacionVendedor;*/

struct Producto {
	int IDUsuario;
	int IDProducto;
	int GLOBAL_IDPRODUCTO = 1;
	string nombreProducto;
	string cantidadProducto;
	string codigoProducto;
	string marcaProducto;
	string descripcionProducto;
	string montoProducto;
	Producto* previo;
	Producto* siguiente;
} *oProducto, *aProducto;

/*struct Envio {
	int IDUsuario;
	int IDProducto;
	int IDEnvio;
};*/

void freeMemory();
void guardarUsuarioID();
void cargarUsuarioID();
void guardarUsuario(Usuario*);
void cargarUsuario();
void guardarProductoID();
void cargarProductoID();
void guardarProducto(Producto*);
void cargarProducto();
string getText(int, HWND);

BOOL CALLBACK iniciodesesion(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK registrodeusuario(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK misenvios(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK informaciondelvendedor(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK nuevoproducto(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK eliminarproductobuscador(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK editarproductobuscador(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK misproductos(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK comprarproductos(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK cancelarenvio(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK editarenvio(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK editarproducto(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK eliminarproducto(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam);


int WINAPI WinMain(HINSTANCE instancia, HINSTANCE legado, LPSTR comando, int modo) {
	cargarUsuarioID();
	cargarProductoID(); 
	oUsuario = aUsuario = NULL;
	oProducto = aProducto = NULL;
	handlerglobal = instancia;
	HWND hIniciarSesion = CreateDialog(instancia, MAKEINTRESOURCE(IDD_INICIARSESION), NULL, iniciodesesion);
	ShowWindow(hIniciarSesion, SW_SHOW);
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

BOOL CALLBACK iniciodesesion(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam) {
	switch (mensaje) {
	case WM_INITDIALOG: {
		cargarUsuario();
		break;
	}

	case WM_COMMAND: {
		switch (LOWORD(wparam)) {
		case IDC_REGISTRATE: {
			HWND hRegistro = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_REGISTRODEUSUARIO), NULL, registrodeusuario);
			ShowWindow(hRegistro, SW_SHOW);
			exitiniciarsesion = false;
			DestroyWindow(handler);
			break;
		}
		case IDC_INICIARSESION: {
			string username = getText(IDC_EDITNOMBREINICIARSESION, handler);
			string password = getText(IDC_EDITCONTRASENAINICIARSESION, handler);
			if (username.compare("") == 0 || password.compare("") == 0) {
				MessageBox(NULL, "Campo vacío", "ERROR", MB_ICONERROR);
				break;
			}
			if (oUsuario != NULL) {
				bool found = true;
				while (aUsuario != NULL) {
					if (aUsuario->username.compare(username) == 0 && aUsuario->password.compare(password) == 0)
						break;
					if (aUsuario->siguiente == NULL) {
						found = false;
						break;
					}
					aUsuario = aUsuario->siguiente;
				}
				if (found == true) {
					iniciarsesion = aUsuario;
					aUsuario = oUsuario;
					HWND hMisEnvios = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_MISENVIOS), NULL, misenvios);
					ShowWindow(hMisEnvios, SW_SHOW);
					DestroyWindow(handler);
					exitiniciarsesion = false;
				}
				else {
					aUsuario = oUsuario;
					MessageBox(NULL, "El usuario no coincide", "ERROR", MB_ICONERROR);
					break;
				}
			}
			else {
				MessageBox(NULL, "No hay usuarios registrados", "ERROR", MB_ICONERROR);
				break;
			}
			break;
		}
		}
		break;
	}
	case WM_CLOSE: {
		exitiniciarsesion = true;
		EndDialog(handler, 0);
		break;
	}
	case WM_DESTROY: {
		if (exitiniciarsesion == true) {
			freeMemory();
			PostQuitMessage(0);
		}
		break;
	}
	}
	return false;
}

BOOL CALLBACK registrodeusuario(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam) {
	switch (mensaje) {
	case WM_COMMAND: {
		switch (LOWORD(wparam)) {
		case IDC_REGRESAR: {
			freeMemory();
			HWND hIniciarSesion = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_INICIARSESION), NULL, iniciodesesion);
			ShowWindow(hIniciarSesion, SW_SHOW);
			DestroyWindow(handler);
			break;
		}
		case IDC_ENTRAR: {
			string username = getText(IDC_EDITNOMBREREGISTRODEUSUARIO, handler);
			string password = getText(IDC_EDITCONTRASENAREGISTRODEUSUARIO, handler);
			if (username.compare("") == 0 || password.compare("") == 0) {
				MessageBox(NULL, "Campo vacío", "ERROR", MB_ICONERROR);
				break;
			}

			if (oUsuario != NULL) { //YA EXISTE POR LO MENOS 1 USUARIO
				bool found = true;
				while (aUsuario->username.compare(username) != 0) {
					if (aUsuario->siguiente == NULL) {
						found = false;
						break;
					}
					aUsuario = aUsuario->siguiente;
				}
				aUsuario = oUsuario;
				if (found) {
					MessageBox(NULL, "Usuario existente", "ERROR", MB_ICONERROR);
					break;
				}
			}
			
			if (oUsuario == NULL) { //PRIMERA VEZ QUE SE REGISTRA UN USUARIO
				oUsuario = new Usuario;
				oUsuario->username.append(username);
				oUsuario->password.append(password);
				oUsuario->IDUsuario = GLOBAL_IDUSUARIO++;
				oUsuario->previo = NULL;
				oUsuario->siguiente = NULL;
			}
			else { //MAS DE 1 USUARIO
				while (aUsuario->siguiente != NULL) {
					aUsuario = aUsuario->siguiente;
					aUsuario->siguiente = new Usuario;
					aUsuario->siguiente->previo = aUsuario;
					aUsuario = aUsuario->siguiente;
					aUsuario->siguiente = NULL;
					aUsuario->IDUsuario = GLOBAL_IDUSUARIO++;
					aUsuario->username.append(username);
					aUsuario->password.append(password);
				}
			}
			guardarUsuario(oUsuario);
			guardarUsuarioID();
			freeMemory();
			aUsuario = oUsuario;
			HWND hIniciarSesion = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_INICIARSESION), NULL, iniciodesesion);
			ShowWindow(hIniciarSesion, SW_SHOW);
			DestroyWindow(handler);
			break;
		}
		}
	}
	case WM_CLOSE: {
		break;
	}
	case WM_DESTROY: {
		break;
	}
	}
	return false;
}

BOOL CALLBACK informaciondelvendedor(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam) {
	switch (mensaje) {
	case WM_COMMAND: {
		switch (LOWORD(wparam)) {
		case IDC_BUTTONAGREGARINFORMACIONDELVENDEDOR: {
			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			char direccion[MAX_PATH] = "";
			ofn.hwndOwner = handler;
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.lpstrFile = direccion;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrDefExt = ".bmp";
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ofn.lpstrFilter = "Mapa de Bits\0*.bmp";
			if (GetOpenFileName(&ofn) == TRUE) {
				HWND hPictureControl = GetDlgItem(handler, IDC_STATICFOTO1INFORMACIONDELVENDEDOR);
				HBITMAP hbitmap = (HBITMAP)LoadImage(NULL, direccion, IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
				SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbitmap);
			}
			break;
		}
		}
		break;
	}
	case WM_CLOSE: {
		break;
	}
	default:
		break;
	}
	return false;
}

BOOL CALLBACK eliminarproducto(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam) {
	switch (mensaje) {
	case WM_COMMAND: {
		break;
	}
	case WM_CLOSE: {
		break;
	}
	default:
		break;
	}
	return false;
}

BOOL CALLBACK editarproducto(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam) {
	switch (mensaje) {
	case WM_COMMAND: {
		break;
	}
	case WM_CLOSE: {
		break;
	}
	default:
		break;
	}
	return false;
}

BOOL CALLBACK editarenvio(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam) {
	switch (mensaje) {
	case WM_COMMAND: {
		break;
	}
	case WM_CLOSE: {
		break;
	}
	default:
		break;
	}
	return false;
}

BOOL CALLBACK cancelarenvio(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam) {
	switch (mensaje) {
	case WM_COMMAND: {
		break;
	}
	case WM_CLOSE: {
		break;
	}
	default:
		break;
	}
	return false;
}

BOOL CALLBACK comprarproductos(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam) {
	switch (mensaje) {
	case WM_COMMAND: {
		break;
	}
	case WM_CLOSE: {
		break;
	}
	default:
		break;
	}
	return false;
}

BOOL CALLBACK misproductos(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam) {
	switch (mensaje) {
	case WM_INITDIALOG: {
		cargarProducto();
		hLBProducto = GetDlgItem(handler, IDC_LISTMISPRODUCTOS);
		hLBProductoSeleccionado = GetDlgItem(handler, IDC_EDITNOMBREMISPRODUCTOS);
		hLBProductoSeleccionado1 = GetDlgItem(handler, IDC_EDITCANTIDADMISPRODUCTOS);
		hLBProductoSeleccionado2 = GetDlgItem(handler, IDC_EDITCODIGOMISPRODUCTOS);
		hLBProductoSeleccionado3 = GetDlgItem(handler, IDC_EDITMARCAMISPRODUCTOS);
		hLBProductoSeleccionado4 = GetDlgItem(handler, IDC_EDITDESCRIPCIONMISPRODUCTOS);
		hLBProductoSeleccionado5 = GetDlgItem(handler, IDC_EDITMONTOMISPRODUCTOS);
		int index = 0;
		while (aProducto != NULL) {
			SendMessage(hLBProducto, LB_ADDSTRING, NULL, (LPARAM)aProducto->nombreProducto.c_str()); //GUARDAMOS EN EL LISTBOX LOS NOMBRES DE LOS PRODUCTOS CREADOS
			SendMessage(hLBProducto, LB_SETITEMDATA, (WPARAM)index, (LPARAM)aProducto->IDProducto);
			aProducto = aProducto->siguiente;
			index++;
		}
		aProducto = oProducto;
	}
	break;

	case WM_COMMAND: {
		switch (LOWORD(wparam)) {
		case IDC_LISTMISPRODUCTOS: {
			if (HIWORD(wparam) == LBN_SELCHANGE) {
				int index = SendMessage(hLBProducto, LB_GETCURSEL, NULL, NULL);
				int IDProducto = SendMessage(hLBProducto, LB_GETITEMDATA, index, NULL);
				bool found = true;

				while (aProducto != NULL) {
					if (aProducto->IDProducto == IDProducto)
						break;
					if (aProducto->siguiente == NULL) {
						found = false;
						break;
					}
					aProducto = aProducto->siguiente;
				}
				if (found == true) {
					SetWindowText(hLBProductoSeleccionado, aProducto->nombreProducto.c_str());
					SetWindowText(hLBProductoSeleccionado1, aProducto->cantidadProducto.c_str());
					SetWindowText(hLBProductoSeleccionado2, aProducto->codigoProducto.c_str());
					SetWindowText(hLBProductoSeleccionado3, aProducto->marcaProducto.c_str());
					SetWindowText(hLBProductoSeleccionado4, aProducto->descripcionProducto.c_str());
					SetWindowText(hLBProductoSeleccionado5, aProducto->montoProducto.c_str());
				}
				aProducto = oProducto;
			}
			break;
		}
		}
		break;
	}
	case WM_CLOSE: {
		break;
	}
	default:
		break;
	}
	return false;
}

BOOL CALLBACK editarproductobuscador(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam) {
	switch (mensaje) {
	case WM_COMMAND: {
		switch (LOWORD(wparam)) {
		case IDC_BUTTONBUSCAREDITARPRODUCTOBUSCADOR: {
			DialogBox(handlerglobal, MAKEINTRESOURCE(IDD_EDITARPRODUCTO), NULL, (DLGPROC)editarproducto);
			break;
		}
		}
	}
	case WM_CLOSE: {
		break;
	}
	default:
		break;
	}
	return false;
}

BOOL CALLBACK eliminarproductobuscador(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam) {
	switch (mensaje) {
	case WM_COMMAND: {
		switch (LOWORD(wparam)) {
		case IDC_BUTTONBUSCARELIMINARPRODUCTOBUSCADOR: {
			DialogBox(handlerglobal, MAKEINTRESOURCE(IDD_ELIMINARPRODUCTO), NULL, (DLGPROC)eliminarproducto);
			break;
		}
		}
	}
	case WM_CLOSE: {
		break;
	}
	default:
		break;
	}
	return false;
}

BOOL CALLBACK nuevoproducto(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam) {
	switch (mensaje) {
	case WM_COMMAND: {
		switch (LOWORD(wparam)) {
		case IDC_BUTTONAGREGAR1NUEVOPRODUCTO: {
			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			char direccion[MAX_PATH] = "";
			ofn.hwndOwner = handler;
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.lpstrFile = direccion;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrDefExt = ".bmp";
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ofn.lpstrFilter = "Mapa de Bits\0*.bmp";
			if (GetOpenFileName(&ofn) == TRUE) {
				HWND hPictureControl = GetDlgItem(handler, IDC_STATICFOTO1NUEVOPRODUCTO);
				HBITMAP hbitmap= (HBITMAP)LoadImage(NULL, direccion, IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
				SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbitmap);
				imagen1cargadaProducto = true;
			}
			break;
		}

		case IDC_BUTTONAGREGAR2NUEVOPRODUCTO: {
			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			char direccion[MAX_PATH] = "";
			ofn.hwndOwner = handler;
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.lpstrFile = direccion;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrDefExt = ".bmp";
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ofn.lpstrFilter = "Mapa de Bits\0*.bmp";
			if (GetOpenFileName(&ofn) == TRUE) {
				HWND hPictureControl = GetDlgItem(handler, IDC_STATICFOTO2NUEVOPRODUCTO);
				HBITMAP hbitmap = (HBITMAP)LoadImage(NULL, direccion, IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
				SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbitmap);
				imagen2cargadaProducto = true;
			}
			break;
		}

		case IDC_BUTTONGUARDARNUEVOPRODUCTO: {
			string nombreProducto = getText(IDC_EDITNOMBRENUEVOPRODUCTO, handler);
			string cantidadProducto = getText(IDC_EDITCANTIDADNUEVOPRODUCTO, handler);
			string codigoProducto = getText(IDC_EDITCODIGONUEVOPRODUCTO, handler);
			string marcaProducto = getText(IDC_EDITMARCANUEVOPRODUCTO, handler);
			string descripcionProducto = getText(IDC_EDITDESCRIPCIONNUEVOPRODUCTO, handler);
			string montoProducto = getText(IDC_EDITMONTONUEVOPRODUCTO, handler);
			if (nombreProducto.compare("") == 0 || cantidadProducto.compare("") == 0 || codigoProducto.compare("") == 0 || marcaProducto.compare("") == 0 || descripcionProducto.compare("") == 0 || montoProducto.compare("") == 0 || imagen1cargadaProducto == 0 || imagen2cargadaProducto == 0) {
				MessageBox(NULL, "Campo vacío", "ERROR", MB_ICONERROR);
				break;
			}
			if (oProducto != NULL) { //YA EXISTE POR LO MENOS 1 PRODUCTO
				bool found = true;
				while (aProducto->nombreProducto.compare(nombreProducto) != 0) {
					if (aProducto->siguiente == NULL) {
						found = false;
						break;
					}
					aProducto = aProducto->siguiente;
				}
				aProducto = oProducto;
				if (found) {
					MessageBox(NULL, "Producto existente", "ERROR", MB_ICONERROR);
					break;
				}
			}

			if (oProducto == NULL) { //PRIMERA VEZ QUE SE REGISTRA UN PRODUCTO
				oProducto = new Producto;
				oProducto->nombreProducto.append(nombreProducto);
				oProducto->cantidadProducto.append(cantidadProducto);
				oProducto->codigoProducto.append(codigoProducto);
				oProducto->marcaProducto.append(marcaProducto);
				oProducto->descripcionProducto.append(descripcionProducto);
				oProducto->montoProducto.append(montoProducto);
				oProducto->IDProducto = GLOBAL_IDPRODUCTO++;
				oProducto->previo = NULL;
				oProducto->siguiente = NULL;
			}
			else { //MAS DE 1 PRODUCTO
				while (aProducto->siguiente != NULL) {
					aProducto = aProducto->siguiente;
					aProducto->siguiente = new Producto;
					aProducto->siguiente->previo = aProducto;
					aProducto = aProducto->siguiente;
					aProducto->siguiente = NULL;
					aProducto->IDProducto = GLOBAL_IDPRODUCTO++;
					aProducto->nombreProducto.append(nombreProducto);
					aProducto->cantidadProducto.append(cantidadProducto);
					aProducto->codigoProducto.append(codigoProducto);
					aProducto->marcaProducto.append(marcaProducto);
					aProducto->descripcionProducto.append(descripcionProducto);
					aProducto->montoProducto.append(montoProducto);
				}
			}
			guardarProducto(oProducto);
			guardarProductoID();
			freeMemory();
			aProducto = oProducto;
			break;
		}
		case ID_INFORMACIONDELVENDEDOR: {
			HWND hInformacionDelVendedor = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_INFORMACIONDELVENDEDOR), NULL, informaciondelvendedor);
			ShowWindow(hInformacionDelVendedor, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_PRODUCTOS_ALTA: {
			HWND hNuevoProducto = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_NUEVOPRODUCTO), NULL, nuevoproducto);
			ShowWindow(hNuevoProducto, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_PRODUCTOS_BAJA: {
			HWND hEliminarProductoBuscador = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_ELIMINARPRODUCTOBUSCADOR), NULL, eliminarproductobuscador);
			ShowWindow(hEliminarProductoBuscador, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_PRODUCTOS_CAMBIO: {
			HWND hEditarProductoBuscador = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_EDITARPRODUCTOBUSCADOR), NULL, editarproductobuscador);
			ShowWindow(hEditarProductoBuscador, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_PRODUCTOS_MOSTRAR: {
			HWND hMisProductos = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_MISPRODUCTOS), NULL, misproductos);
			ShowWindow(hMisProductos, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_ENVIOS_COMPRAR: {
			HWND hComprarProductos = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_COMPRARPRODUCTOS), NULL, comprarproductos);
			ShowWindow(hComprarProductos, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_ENVIOS_CANCELAR: {
			HWND hCancelarEnvio = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_CANCELARENVIO), NULL, cancelarenvio);
			ShowWindow(hCancelarEnvio, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_ENVIOS_EDITAR: {
			HWND hEditarEnvio = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_EDITARENVIO), NULL, editarenvio);
			ShowWindow(hEditarEnvio, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_ENVIOS_MOSTRAR: {
			HWND hMisEnvios = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_MISENVIOS), NULL, misenvios);
			ShowWindow(hMisEnvios, SW_SHOW);
			DestroyWindow(handler);
			break;
		}
		}
	}
	case WM_CLOSE: {
		break;
	}
	default:
		break;
	}
	return false;
}

BOOL CALLBACK misenvios(HWND handler, UINT mensaje, WPARAM wparam, LPARAM lparam) {
	switch (mensaje) {
	case WM_INITDIALOG: {
		
		break;
	}
	case WM_COMMAND: {
		switch (LOWORD(wparam)) {
		case ID_INFORMACIONDELVENDEDOR: {
			HWND hInformacionDelVendedor = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_INFORMACIONDELVENDEDOR), NULL, informaciondelvendedor);
			ShowWindow(hInformacionDelVendedor, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_PRODUCTOS_ALTA: {
			HWND hNuevoProducto = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_NUEVOPRODUCTO), NULL, nuevoproducto);
			ShowWindow(hNuevoProducto, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_PRODUCTOS_BAJA: {
			HWND hEliminarProductoBuscador = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_ELIMINARPRODUCTOBUSCADOR), NULL, eliminarproductobuscador);
			ShowWindow(hEliminarProductoBuscador, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_PRODUCTOS_CAMBIO: {
			HWND hEditarProductoBuscador = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_EDITARPRODUCTOBUSCADOR), NULL, editarproductobuscador);
			ShowWindow(hEditarProductoBuscador, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_PRODUCTOS_MOSTRAR: {
			HWND hMisProductos = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_MISPRODUCTOS), NULL, misproductos);
			ShowWindow(hMisProductos, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_ENVIOS_COMPRAR: {
			HWND hComprarProductos = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_COMPRARPRODUCTOS), NULL, comprarproductos);
			ShowWindow(hComprarProductos, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_ENVIOS_CANCELAR: {
			HWND hCancelarEnvio = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_CANCELARENVIO), NULL, cancelarenvio);
			ShowWindow(hCancelarEnvio, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_ENVIOS_EDITAR: {
			HWND hEditarEnvio = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_EDITARENVIO), NULL, editarenvio);
			ShowWindow(hEditarEnvio, SW_SHOW);
			DestroyWindow(handler);
			break;
		}

		case ID_ENVIOS_MOSTRAR: {
			HWND hMisEnvios = CreateDialog(handlerglobal, MAKEINTRESOURCE(IDD_MISENVIOS), NULL, misenvios);
			ShowWindow(hMisEnvios, SW_SHOW);
			DestroyWindow(handler);
			break;
		}
		}
		break;
	}
	case WM_CLOSE: {
		break;
	}
	case WM_DESTROY: {
		break;
	}
	}
	return false;
}

string getText(int elemento, HWND ventana) {
	HWND handler = GetDlgItem(ventana, elemento);
	int textLenght = GetWindowTextLength(handler);
	if (textLenght < 1)
		return "";
	char texto[300];
	GetWindowText(handler, texto, ++textLenght);
	string stexto(texto);
	return stexto;
}

void guardarUsuarioID() {
	archivo.open("GlobalUsuarioID.txt", ios::out | ios::trunc);
	if (archivo.is_open()) {
		archivo << GLOBAL_IDUSUARIO;
		MessageBox(NULL, "ID de usuario global guardado exitosamente", "EXITO", MB_ICONINFORMATION);
		archivo.close();
		return;
	}
	else {
		MessageBox(NULL, "El archivo no se puedo abrir", "ERROR", MB_ICONERROR);
		return;
	}
}

void cargarUsuarioID() {
	archivo.open("GlobalUsuarioID.txt", ios::in);
	if (archivo.is_open()) {
		archivo >> GLOBAL_IDUSUARIO;
		MessageBox(NULL, "ID de usuario global cargado exitosamente", "EXITO", MB_ICONINFORMATION);
		archivo.close();
		return;
	}
	else {
		MessageBox(NULL, "El archivo no se puedo abrir", "ERROR", MB_ICONERROR);
		return;
	}
}

void guardarUsuario(Usuario* origen) {
	archivo.open("Usuarios.bin", ios::binary | ios::out | ios::trunc);
	if (archivo.is_open()) {
		while (origen != NULL) {
			archivo.write(reinterpret_cast<char*>(origen), sizeof(Usuario));
			origen = origen->siguiente;
		}
		MessageBox(NULL, "Usuario guardado exitosamente", "EXITO", MB_ICONINFORMATION);
		archivo.close();
		return;
	}
	else {
		MessageBox(NULL, "El archivo no se puedo abrir", "ERROR", MB_ICONERROR);
		return;
	}
}

void cargarUsuario() {
	archivo.open("Usuarios.bin", ios::binary | ios::in | ios::ate);
	if (archivo.is_open()) {
		int totalChar = archivo.tellg();
		if (totalChar < 1) {
			MessageBox(NULL, "Archivo vacío", "ERROR", MB_ICONERROR);
			return;
		}
		for (int i = 0; i < totalChar / sizeof(Usuario); i++) {
			if (oUsuario == NULL) {
				Usuario* temp = new Usuario;
				oUsuario = new Usuario;
				archivo.seekg(i * sizeof(Usuario));
				archivo.read(reinterpret_cast<char*>(temp), sizeof(Usuario));
				oUsuario = new Usuario;
				oUsuario->username.append(temp->username);
				oUsuario->password.append(temp->password);
				oUsuario->IDUsuario = temp->IDUsuario;
				oUsuario->previo = NULL;
				oUsuario->siguiente = NULL;
				aUsuario = oUsuario;
				delete reinterpret_cast<char*>(temp);
				continue;
			}
			else {
				while (aUsuario->siguiente != NULL) {
					aUsuario = aUsuario->siguiente;
					Usuario* temp = new Usuario;
					aUsuario->siguiente = new Usuario;
					archivo.seekg(i * sizeof(Usuario));
					archivo.read(reinterpret_cast<char*>(temp), sizeof(Usuario));
					aUsuario->siguiente->previo = aUsuario;
					aUsuario = aUsuario->siguiente;
					aUsuario->siguiente = NULL;
					aUsuario->username.append(temp->username);
					aUsuario->password.append(temp->password);
					aUsuario->IDUsuario = temp->IDUsuario;
					aUsuario = oUsuario;
					delete reinterpret_cast<char*>(temp);
					continue;
				}
			}
		}
		MessageBox(NULL, "Carga de usuarios exitoso", "EXITO", MB_ICONINFORMATION);
		archivo.close();
		return;
	}
	else {
		MessageBox(NULL, "El archivo no se puedo abrir", "ERROR", MB_ICONERROR);
		return;
	}
}

void guardarProductoID() {
	archivo.open("GlobalProductoID.txt", ios::out | ios::trunc);
	if (archivo.is_open()) {
		archivo << GLOBAL_IDPRODUCTO;
		MessageBox(NULL, "ID de producto global guardado exitosamente", "EXITO", MB_ICONINFORMATION);
		archivo.close();
		return;
	}
	else {
		MessageBox(NULL, "El archivo no se puedo abrir", "ERROR", MB_ICONERROR);
		return;
	}
}

void cargarProductoID() {
	archivo.open("GlobalProductoID.txt", ios::in);
	if (archivo.is_open()) {
		archivo >> GLOBAL_IDPRODUCTO;
		MessageBox(NULL, "ID de producto global cargado exitosamente", "EXITO", MB_ICONINFORMATION);
		archivo.close();
		return;
	}
	else {
		MessageBox(NULL, "El archivo no se puedo abrir", "ERROR", MB_ICONERROR);
		return;
	}
}

void guardarProducto(Producto* origen) {
	archivo.open("Productos.bin", ios::binary | ios::out | ios::trunc);
	if (archivo.is_open()) {
		while (origen != NULL) {
			archivo.write(reinterpret_cast<char*>(origen), sizeof(Producto));
			origen = origen->siguiente;
		}
		MessageBox(NULL, "Producto guardado exitosamente", "EXITO", MB_ICONINFORMATION);
		archivo.close();
		return;
	}
	else {
		MessageBox(NULL, "El archivo no se puedo abrir", "ERROR", MB_ICONERROR);
		return;
	}
}

void cargarProducto() {
	archivo.open("Productos.bin", ios::binary | ios::in | ios::ate);
	if (archivo.is_open()) {
		int totalChar = archivo.tellg();
		if (totalChar < 1) {
			MessageBox(NULL, "Archivo vacío", "ERROR", MB_ICONERROR);
			return;
		}
		for (int i = 0; i < totalChar / sizeof(Producto); i++) {
			if (oProducto == NULL) {
				Producto* temp = new Producto;
				oProducto = new Producto;
				archivo.seekg(i * sizeof(Producto));
				archivo.read(reinterpret_cast<char*>(temp), sizeof(Producto));
				oProducto = new Producto;
				oProducto->nombreProducto.append(temp->nombreProducto);
				oProducto->cantidadProducto.append(temp->cantidadProducto);
				oProducto->codigoProducto.append(temp->codigoProducto);
				oProducto->marcaProducto.append(temp->marcaProducto);
				oProducto->descripcionProducto.append(temp->descripcionProducto);
				oProducto->montoProducto.append(temp->montoProducto);
				oProducto->IDProducto = temp->IDProducto;
				oProducto->previo = NULL;
				oProducto->siguiente = NULL;
				aProducto = oProducto;
				delete reinterpret_cast<char*>(temp);
				continue;
			}
			else {
				while (aProducto->siguiente != NULL) {
					aProducto = aProducto->siguiente;
					Producto* temp = new Producto;
					aProducto->siguiente = new Producto;
					archivo.seekg(i * sizeof(Producto));
					archivo.read(reinterpret_cast<char*>(temp), sizeof(Producto));
					aProducto->siguiente->previo = aProducto;
					aProducto = aProducto->siguiente;
					aProducto->siguiente = NULL;
					aProducto->nombreProducto.append(temp->nombreProducto);
					aProducto->cantidadProducto.append(temp->cantidadProducto);
					aProducto->codigoProducto.append(temp->codigoProducto);
					aProducto->marcaProducto.append(temp->marcaProducto);
					aProducto->descripcionProducto.append(temp->descripcionProducto);
					aProducto->montoProducto.append(temp->montoProducto);
					aProducto->IDProducto = temp->IDProducto;
					aProducto = oProducto;
					delete reinterpret_cast<char*>(temp);
					continue;
				}
			}
		}
		MessageBox(NULL, "Carga de Productos exitoso", "EXITO", MB_ICONINFORMATION);
		archivo.close();
		return;
	}
	else {
		MessageBox(NULL, "El archivo no se puedo abrir", "ERROR", MB_ICONERROR);
		return;
	}
}

void freeMemory() {
	if (oUsuario != NULL) {
		while (aUsuario != NULL) {
			Usuario* temp = aUsuario;
			aUsuario = aUsuario->siguiente;
			delete temp;
		}
		aUsuario = oUsuario = NULL;
	}
	else {
		return;
	}
}