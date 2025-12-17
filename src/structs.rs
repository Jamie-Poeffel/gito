#[derive(Debug)]
pub struct Template {
    pub name: String,
    pub questions: Vec<Question>,
    pub build: String,
}

#[derive(Clone, Debug)]
pub struct Question {
    pub id: String,
    pub text: String,
    pub kind: QuestionType,
    pub before: String,
    pub options: Option<Vec<String>>,
    pub default: Option<String>,
    pub optional: bool,
    pub includes: Option<Includes>,
}

#[derive(Clone, Debug)]
pub enum QuestionType {
    String,
    Number,
}

#[derive(Clone, Debug)]
pub enum Includes {
    Brackets,
    None
}