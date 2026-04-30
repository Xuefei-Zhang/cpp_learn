import test from 'node:test';
import assert from 'node:assert/strict';

import {
  buildStemIndex,
  normalizeRepoPath,
  resolveWikiTarget,
  extractWikiLinksOutsideCodeFences,
} from './preview-core.mjs';

const manifestFiles = [
  'README.md',
  '2026-04-07-language-mastery-lab-generic-design.md',
  'docs/phases/phase-a-engineering-foundation-and-object-model.md',
  'docs/phases/phase-b-generic-design-and-interface-quality.md',
  'docs/phases/phase-c-runtime-mechanics-performance-and-concurrency.md',
  'docs/phases/phase-d-system-integration-and-portfolio-project.md',
  'docs/glossary/week-01-core-terms.md',
  'docs/superpowers/specs/2026-04-08-cpp-engineering-mastery-lab-design.md',
  'docs/superpowers/plans/2026-04-08-cpp-engineering-mastery-lab-implementation-plan.md',
  'missions/week-01-project-bootstrap-and-value-semantics/README.md',
  'missions/week-02-raii-and-resource-management/README.md',
  'notes/weekly/week-01.md',
  'notes/weekly/week-02.md',
];

test('normalizeRepoPath keeps repo-relative markdown paths normalized', () => {
  assert.equal(
    normalizeRepoPath('docs/phases/../glossary/week-01-core-terms.md'),
    'docs/glossary/week-01-core-terms.md',
  );
});

test('resolveWikiTarget resolves relative mission README links from phase docs', () => {
  const stemIndex = buildStemIndex(manifestFiles);
  const resolved = resolveWikiTarget({
    currentFile: 'docs/phases/phase-a-engineering-foundation-and-object-model.md',
    targetRaw: '../../missions/week-01-project-bootstrap-and-value-semantics/README',
    stemIndex,
    knownFiles: new Set(manifestFiles),
  });

  assert.deepEqual(resolved, {
    ok: true,
    path: 'missions/week-01-project-bootstrap-and-value-semantics/README.md',
  });
});

test('resolveWikiTarget resolves sibling glossary links from phase docs', () => {
  const stemIndex = buildStemIndex(manifestFiles);
  const resolved = resolveWikiTarget({
    currentFile: 'docs/phases/phase-a-engineering-foundation-and-object-model.md',
    targetRaw: '../glossary/week-01-core-terms',
    stemIndex,
    knownFiles: new Set(manifestFiles),
  });

  assert.deepEqual(resolved, {
    ok: true,
    path: 'docs/glossary/week-01-core-terms.md',
  });
});

test('resolveWikiTarget resolves bare repo-root stem links via manifest index', () => {
  const stemIndex = buildStemIndex(manifestFiles);
  const resolved = resolveWikiTarget({
    currentFile: 'docs/superpowers/specs/2026-04-08-cpp-engineering-mastery-lab-design.md',
    targetRaw: '2026-04-07-language-mastery-lab-generic-design',
    stemIndex,
    knownFiles: new Set(manifestFiles),
  });

  assert.deepEqual(resolved, {
    ok: true,
    path: '2026-04-07-language-mastery-lab-generic-design.md',
  });
});

test('extractWikiLinksOutsideCodeFences ignores [[nodiscard]] inside fenced code', () => {
  const markdown = [
    '# Example',
    '',
    'A real link: [[../glossary/week-01-core-terms|Week 1 Core Terms]]',
    '',
    '```cpp',
    '[[nodiscard]] constexpr int size() const noexcept;',
    '```',
  ].join('\n');

  const links = extractWikiLinksOutsideCodeFences(markdown);
  assert.deepEqual(links, [
    {
      raw: '../glossary/week-01-core-terms|Week 1 Core Terms',
      target: '../glossary/week-01-core-terms',
      alias: 'Week 1 Core Terms',
    },
  ]);
});
