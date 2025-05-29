#include <crow.h>
#include <cstdlib>
#include <dotenv.h>
#include <pqxx/pqxx>
#include <stdexcept>

void initDB(pqxx::connection &C) {
  pqxx::work W(C);

  W.exec(R"(
    create table if not exists todos (
      id serial primary key,
      todo text not null,
			completed boolean default false
    );
	)");

  W.commit();
}

crow::json::wvalue getTodos(pqxx::connection &C) {
  pqxx::work W(C);
  pqxx::result res = W.exec(R"(select id, todo, completed from todos)");

  crow::json::wvalue todos = crow::json::wvalue::list();

  int idx = 0;

  for (const auto row : res) {
    todos[idx]["id"] = row["id"].as<int>();
    todos[idx]["todo"] = row["todo"].as<std::string>();
    todos[idx]["completed"] = row["completed"].as<bool>();
    idx++;
  }

  return todos;
}

void createTask(pqxx::connection &C, std::string &task, bool &completed) {
  pqxx::work W(C);
  W.exec_params(R"(insert into todos(todo, completed) values($1, $2))", task, completed);
  W.commit();
}

void deleteTask(pqxx::connection &C, int &id) {
  pqxx::work W(C);

  W.exec_params(R"(delete from todos where id = $1)", id);
  W.commit();
}

void updateTask(pqxx::connection &C, int &id, std::string &todo, bool &completed) {
  pqxx::work W(C);
  W.exec_params(R"(update todos set todo = $2, completed = $3 where id = $1)", id, todo, completed);
  W.commit();
}

int main() {
  dotenv::init();
  crow::SimpleApp App;

  pqxx::connection C(std::getenv("DATABASE_URL"));

  if (!C.is_open()) throw std::runtime_error("Failed to connect DB");

  initDB(C);

  CROW_ROUTE(App, "/api/todos").methods(crow::HTTPMethod::GET)([&C]() { return crow::response(200, getTodos(C)); });

  CROW_ROUTE(App, "/api/todos").methods(crow::HTTPMethod::PUT)([&C](const crow::request &req) {
    const auto body = crow::json::load(req.body);
    if (!body) return crow::response(400, "Invalid Json");

    int id = body["id"].i();
    std::string todo = body["todo"].s();
    bool completed = body["completed"].b();

    updateTask(C, id, todo, completed);
    return crow::response(200, "Updated Task");
  });

  CROW_ROUTE(App, "/api/todos").methods(crow::HTTPMethod::POST)([&C](const crow::request &req) {
    const auto body = crow::json::load(req.body);
    if (!body) return crow::response(400, "Invalid Json");

    std::string todo = body["todo"].s();
    bool completed = body["completed"].b();

    createTask(C, todo, completed);

    return crow::response(200, "Todo received");
  });

  CROW_ROUTE(App, "/api/todos/<int>").methods(crow::HTTPMethod::DELETE)([&C](int id) {
    deleteTask(C, id);
    return crow::response(200, "Deleted Todo");
  });

  App.port(3000).run();
  return 0;
}
