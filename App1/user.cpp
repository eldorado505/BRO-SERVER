#include "broServer.cpp"
// Bobby [The Web Application Developer] - user of web server.
int main() {
	struct student {
		int rollNumber;
		char name[21];
		char gender;
	};
	Bro bro;
	try {
		bro.setStaticResourcesFolder("./main-files");
	}
	catch (string exception) {
		cout << exception << endl;
		return 0;
	}
	bro.get("/", [](Request & request, Response & response)->void {
		response << R""""(
		<!DOCTYPE html>
		<html lang='en'>
		<head>
		    <meta charset='UTF-8'>
		    <meta http-equiv='X-UA-Compatible" content="IE=edge'>
		    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
		    <title>Thinking Machines</title>
		</head>
		<body>
		    <h1>List Of Students</h1>
		    <table border='1'>
		        <thead >
		            <tr>
		                <th>S.No</th>
		                <th>Roll No</th>
		                <th>Name</th>
		                <th>Gender</th>
		                <th>Edit</th>
		                <th>Delete</th>
		            </tr>
		        </thead>
		        <tbody>
		)"""";
		FILE *file = fopen("/student.dat", "rb");
		int sno = 0;
		if (file != NULL) {
			student stud;
			while (1) {
				fread(&stud, sizeof(stud), 1, file);
				if (feof(file)) break;
				sno++;
				response << "<tr>";
				response << "<td>" << to_string(sno).c_str() << "</td>";
				response << "<td>" << to_string(stud.rollNumber).c_str() << "</td>";
				response << "<td>" << stud.name << "</td>";
				if (stud.gender == 'M') {
					response << "<td><img src='/images/male.jpg'></td>";
				}
				else {
					response << "<td><img src='/images/female.png'></td>";
				}
				response << "<td><a href='/editStudent?rollNumber=" << to_string(stud.rollNumber).c_str() << "'>Edit</a></td>";
				response << "<td><a href='/confirmStudentDeletion?rollNumber=" << to_string(stud.rollNumber).c_str() << "'>Delete</a></td>";
				response << "</tr>";
			}// loop ends
			fclose(file);
		}

		if (sno == 0) {
			response << "<tr><td span='6' align='center'>No Students</td></tr>";
		}
		response << R""""(
			</tbody>
			</table>
			<br>
			<a href='/StudentAddForm.html'>Add Student</a>
			</body>
			</html>
			)"""";
	});
	bro.get("/addStudent", [](Request & request, Response & response)->void {
		string rollNumber = request["rollNumber"];
		string name = request["name"];
		string gender = request["gender"];
		// check if roll no already exists starts here
		int vRollNumber = stoi(rollNumber);
		FILE *file = fopen("/student.dat", "rb");
		if (file != NULL) {
			bool found = false;
			student studTemp;
			while (1) {
				fread(&studTemp, sizeof(student), 1, file);
				if (feof(file)) break;
				if (studTemp.rollNumber == vRollNumber) {
					found = true;
					break;
				}
			}
			fclose(file);
			if (found) {
				//create html with form and fill all the arrived student's info and send it back as response.
				response << R""""(
				<!DOCTYPE html>
				<html lang='en'>
				<head>
				    <meta charset='UTF-8'>
				    <meta http-equiv='X-UA-Compatible' content='IE=edge'>
				    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
				    <title>Document</title>
				    <script>
				        function validateForm(form) {
				            var rollNumber=form.rollNumber.value.trim();
				            if(rollNumber.length==0) {
				                alert('Roll Number Required');
				                form.rollNumber.focus();
				                return false;
				            }
				            rollNumber=parseInt(rollNumber);
				            if(isNaN(rollNumber)) {
				                alert('Roll Number must be integer');
				                form.rollNumber.value='';
				                form.rollNumber.focus();
				                return false;
				            }
				            if(rollNumber<=0) {
				                alert('Roll Number must be non-zero');
				                form.rollNumber.value='';
				                form.rollNumber.focus();
				                return false;
				            }
				            var name=form.name.value.trim();
				            if(name.length==0) {
				                alert('Name Required');
				                form.name.focus();
				                return false;
				            }
				            var allow='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz. ';
				            var i=0;
				            while(i<name.length) {
				                if(allow.indexOf(name.charAt(i))==-1) {
				                    alert('Invalid characters in name');
				                    form.name.focus();
				                    return false;
				                }
				                i++;
				            }
				            return true;
				        }
				    </script>
				</head>
				<body>
				    <h1>Student (Add Module)</h1>)"""";
				char line[201];
				sprintf(line, "<div style='color:red'>Roll Number %d is already alloted</div>", vRollNumber);
				response << line;
				response << R""""(
				    <form action='/addStudent' method='get' onsubmit='return validateForm(this)'> 
				        <div>)"""";
				sprintf(line, "RollNumber: <input type='text' name='rollNumber' id='rollNumber' value='%d'>", vRollNumber);
				response << line;
				response << R""""(
				        </div>
				        <div> )"""";
				sprintf(line, "Name: <input type='text' name='name' id='name' value='%s'>", name.c_str());
				response << line;
				response << R""""(
				        </div>
				        <div>)"""";
				if (gender[0] == 'M') {
					response << "Gender: &nbsp;&nbsp;&nbsp;Male<input type='radio' name='gender' id='male' value='M' checked> Female <input type='radio' name='gender' id='female' value='F'>";
				}
				else {
					response << "Gender: &nbsp;&nbsp;&nbsp;Male<input type='radio' name='gender' id='male' value='M' > Female <input type='radio' name='gender' id='female' value='F' checked>";
				}
				response << R""""(
				        </div>
				        <div>
				            <button type='submit'>Add</button>
				        </div>
				    </form>
				    <br>
				    <a href='/'>Home</a>
				</body>
				</html>
				)"""";
				return;
			}
		}
		// check if roll no already exists ends here
		student stud;
		stud.rollNumber = stoi(rollNumber);
		strcpy(stud.name, name.c_str());
		stud.gender = gender[0];
		file = fopen("/student.dat", "ab");
		fwrite(&stud, sizeof(stud), 1, file);
		fclose(file);
		response << R""""(
		<!DOCTYPE html>
		<html lang='en'>
		<head>
		    <meta charset='UTF-8'>
		    <meta http-equiv='X-UA-Compatible' content='IE=edge'>
		    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
		    <title>Thinking Machine</title>
		</head>
		<body>
		    <h1>Student (Add Module)</h1>
		    <h3>Student Added</h3>
		    <br>
		    <form action='/'>
		        <button type='submit'>OK</button>
		    </form>
		    <br>
		    <a href='/'>Home</a>
		</body>
		</html>
		)"""";
	});
	bro.get("/editStudent", [](Request & request, Response & response)->void {
		string rollNumber = request["rollNumber"];
		int vRollNumber = stoi(rollNumber);
		FILE *f = fopen("/student.dat", "rb");
		student stud;
		while (1) {
			fread(&stud, sizeof(student), 1, f);
			if (feof(f)) break;
			if (stud.rollNumber == vRollNumber) break;
		}
		fclose(f);
		response << R""""(
		<!DOCTYPE html>
		<html lang='en'>
		<head>
		    <meta charset='UTF-8'>
		    <meta http-equiv='X-UA-Compatible' content='IE=edge'>
		    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
		    <title>Document</title>
		    <script>
		        function validateForm(form) {
		            var name=form.name.value.trim();
		            if(name.length==0) {
		                alert('Name Required');
		                form.name.focus();
		                return false;
		            }
		            var allow='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz. ';
		            var i=0;
		            while(i<name.length) {
		                if(allow.indexOf(name.charAt(i))==-1) {
		                    alert('Invalid characters in name');
		                    form.name.focus();
		                    return false;
		                }
		                i++;
		            }
		            return true;
		        }
		    </script>
		</head>
		<body>
		    <h1>Student (Edit Module)</h1>
		    <form action='/updateStudent' method='get' onsubmit='return validateForm(this)'>
		        <div>)"""";
		char line[201];
		sprintf(line, "<input type='hidden' name='rollNumber' id='rollNumber' value='%d'>\n Roll Number: %d", stud.rollNumber, stud.rollNumber);
		response << line;
		response << R""""(
		        </div>
		        <div>)"""";
		sprintf(line, "Name: <input type='text' name='name' id='name' value='%s'>", stud.name);
		response << line;
		response << R""""(
		        </div>
		        <div>)"""";
		if (stud.gender == 'M') {
			strcpy(line, "Gender: &nbsp;&nbsp;&nbsp;Male<input type='radio' name='gender' id='male' value='M' checked> Female <input type='radio' name='gender' id='female' value='F'>");
		}
		else {
			strcpy(line, "Gender: &nbsp;&nbsp;&nbsp;Male<input type='radio' name='gender' id='male' value='M'> Female <input type='radio' name='gender' id='female' value='F' checked>");
		}
		response << line;
		response << R""""(
		        </div>
		        <div>
		            <button type='Update'>Add</button>
		        </div>
		    </form>
		    <br>
		    <a href='/'>Home</a>
		</body>
		</html>
		)"""";
	});
	bro.get("/updateStudent", [](Request & request, Response & response)->void {
		string rollNumber = request["rollNumber"];
		string name = request["name"];
		string gender = request["gender"];
		student stud;
		stud.rollNumber = stoi(rollNumber);
		strcpy(stud.name, name.c_str());
		stud.gender = gender[0];
		// Update the file
		FILE *f1 = fopen("/student.dat", "rb");
		FILE *f2 = fopen("/temp.dat", "wb");
		student stud2;
		while (1) {
			fread(&stud2, sizeof(student), 1, f1);
			if (feof(f1)) break;
			if (stud2.rollNumber == stud.rollNumber) fwrite(&stud, sizeof(student), 1, f2);
			else fwrite(&stud2, sizeof(student), 1, f2);
		}
		fclose(f1);
		fclose(f2);
		f1 = fopen("/student.dat", "wb");
		f2 = fopen("/temp.dat", "rb");
		while (1) {
			fread(&stud2, sizeof(student), 1, f2);
			if (feof(f2)) break;
			fwrite(&stud2, sizeof(student), 1, f1);
		}
		fclose(f1);
		fclose(f2);
		f2 = fopen("/temp.dat", "wb");
		fclose(f2);
		response << R""""(
		<!DOCTYPE html>
		<html lang='en'>
		<head>
		    <meta charset='UTF-8'>
		    <meta http-equiv='X-UA-Compatible' content='IE=edge'>
		    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
		    <title>Thinking Machine</title>
		</head>
		<body>
		    <h1>Student (Edit Module)</h1>
		    <h3>Student Updated</h3>
		    <br>
		    <form action='/'>
		        <button type='submit'>OK</button>
		    </form>
		    <br>
		    <a href='/'>Home</a>
		</body>
		</html>
		)"""";

	});
	bro.get("/confirmStudentDeletion", [](Request & request, Response & response)->void {
		string rollNumber = request["rollNumber"];
		int vRollNumber = stoi(rollNumber);
		student stud;
		FILE *file = fopen("/student.dat", "rb");
		while (1) {
			fread(&stud, sizeof(student), 1, file);
			if (feof(file)) break;
			if (stud.rollNumber == vRollNumber) break;
		}
		fclose(file);
		// serve response
		response << R""""(
		<!DOCTYPE HTML>
		<html lang='en'>
		<head>
		<meta charset='utf-8'>
		<title>Confirm Delete</title>
		</head>
		<body>
		<h1>Student (Delete Module)</h1>
		<form action='/deleteStudent'>
		Roll Number : )"""";
		char str[11];
		itoa(stud.rollNumber, str, 10);
		response << str;
		response << "<br/>Name :";
		char line[101];
		sprintf(line, "<input type='hidden' id='rollNumber' name='rollNumber' value='%d'>", vRollNumber);
		response << line;
		response << stud.name;
		response << "<br/>Gender :";
		if (stud.gender == 'M') response << "Male";
		else response << "Female";
		response << R""""(
		<br/>
		<button type='submit'>Delete</button>
		</form>
		<br/>
		<a href='/'>Home</a>
		</body>
		</html>
		)"""";
	});

	bro.get("/deleteStudent", [](Request & request, Response & response)->void {
		string rollNumber = request["rollNumber"];
		int vRollNumber = stoi(rollNumber);
		// Delete record from file
		FILE *f1 = fopen("/student.dat", "rb");
		FILE *f2 = fopen("/temp.dat", "wb");
		student stud2;
		while (1) {
			fread(&stud2, sizeof(student), 1, f1);
			if (feof(f1)) break;
			if (stud2.rollNumber != vRollNumber) fwrite(&stud2, sizeof(student), 1, f2);
		}
		fclose(f1);
		fclose(f2);
		f1 = fopen("/student.dat", "wb");
		f2 = fopen("/temp.dat", "rb");
		while (1) {
			fread(&stud2, sizeof(student), 1, f2);
			if (feof(f2)) break;
			fwrite(&stud2, sizeof(student), 1, f1);
		}
		fclose(f1);
		fclose(f2);
		f2 = fopen("/temp.dat", "wb");
		fclose(f2);
		response << R""""(
		<!DOCTYPE html>
		<html lang='en'>
		<head>
		    <meta charset='UTF-8'>
		    <meta http-equiv='X-UA-Compatible' content='IE=edge'>
		    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
		    <title>Deletion Successfully</title>
		</head>
		<body>
		    <h1>Student (Delete Module)</h1>
		    <h3>Student Deleted</h3>
		    <br>
		    <form action='/'>
		        <button type='submit'>OK</button>
		    </form>
		    <br>
		    <a href='/'>Home</a>
		</body>
		</html>
		)"""";

	});
	bro.listen(6060, [](Error & error)->void {
		if (error.hasError()) {
			cout << error.getError() << endl;
			return;
		}
		cout << "Bro Web Server is ready to accept the request on port 6060" << endl;
	});
	return 0;
}
