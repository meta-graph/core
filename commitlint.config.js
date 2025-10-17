module.exports = {
  rules: {
    'type-enum': [2, 'always', ['build', 'chore', 'ci', 'docs', 'feat', 'fix', 'refactor', 'test']],
    'type-case': [2, 'always', 'lower-case'],
    'subject-empty': [2, 'never'],
    'header-max-length': [2, 'always', 72]
  }
};
