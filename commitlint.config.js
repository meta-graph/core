module.exports = {
  extends: ['@commitlint/config-conventional'],
  ignores: [
    (message) => /^Update\b/.test(message)
  ],
  rules: {
    'header-max-length': [2, 'always', 72]
  }
};
