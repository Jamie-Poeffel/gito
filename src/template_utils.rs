#[derive(Debug, PartialEq, Eq, Hash)]
pub enum Key {
    NAME,
    QUESTION,
    TYPE,
    ID,
    OPTIONS,
    DEFAULT,
    BEFORE,
    OPTIONAL,
    INCLUDES,
    BUILD,
}

impl Key {
    pub fn from_str(s: &str) -> Result<Self, String> {
        match s {
            "NAME" => Ok(Key::NAME),
            "QUESTION" => Ok(Key::QUESTION),
            "TYPE" => Ok(Key::TYPE),
            "ID" => Ok(Key::ID),
            "OPTIONS" => Ok(Key::OPTIONS),
            "DEFAULT" => Ok(Key::DEFAULT),
            "BEFORE" => Ok(Key::BEFORE),
            "OPTIONAL" => Ok(Key::OPTIONAL),
            "INCLUDES" => Ok(Key::INCLUDES),
            "BUILD" => Ok(Key::BUILD),
            _ => Err(format!("Unknown key or not uppercase: {}", s)),
        }
    }
}
