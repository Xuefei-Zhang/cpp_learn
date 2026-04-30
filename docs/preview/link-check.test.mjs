import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs/promises';
import path from 'node:path';

import {
  buildStemIndex,
  extractWikiLinksOutsideCodeFences,
  resolveWikiTarget,
} from './preview-core.mjs';

const repoRoot = '/home/xuefeiz2/self/cpp_data_structure';
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

const expectedMissingLinks = [
  '../../missions/week-03-small-container-and-exception-safety/README|Week 3 Mission',
  '../../missions/week-04-templates-constraints-and-generic-apis/README|Week 4 Mission',
  '../../missions/week-05-callable-design-and-type-erasure/README|Week 5 Mission',
  '../../missions/week-06-iterators-views-and-range-oriented-design/README|Week 6 Mission',
  '../../missions/week-07-allocators-and-memory-resource-thinking/README|Week 7 Mission',
  '../../missions/week-08-concurrency-fundamentals/README|Week 8 Mission',
  '../../missions/week-09-exception-safety-and-performance-accountability/README|Week 9 Mission',
  '../../missions/week-10-task-runtime-skeleton/README|Week 10 Mission',
  '../../missions/week-11-resource-aware-execution-pipeline/README|Week 11 Mission',
  '../../missions/week-12-productization-explanation-and-evidence/README|Week 12 Mission',
  '../glossary/week-04-generic-design-terms|Week 4 Generic Design Terms',
  '../glossary/week-07-runtime-and-allocation-terms|Week 7 Runtime and Allocation Terms',
  '../glossary/week-10-system-integration-terms|Week 10 System Integration Terms',
].sort();

test('repo wiki links either resolve now or are part of the known future-doc gap', async () => {
  const knownFiles = new Set(manifestFiles);
  const stemIndex = buildStemIndex(manifestFiles);
  const missing = new Set();
  const unexpectedFailures = [];

  for (const file of manifestFiles) {
    const absPath = path.join(repoRoot, file);
    const content = await fs.readFile(absPath, 'utf8');
    const links = extractWikiLinksOutsideCodeFences(content);

    for (const link of links) {
      const resolved = resolveWikiTarget({
        currentFile: file,
        targetRaw: link.target,
        stemIndex,
        knownFiles,
      });

      if (!resolved.ok) {
        if (resolved.reason === 'missing-target') {
          missing.add(link.raw);
        } else {
          unexpectedFailures.push({ file, link: link.raw, reason: resolved.reason, candidates: resolved.candidates ?? [] });
        }
      }
    }
  }

  assert.deepEqual(unexpectedFailures, []);
  assert.deepEqual([...missing].sort(), expectedMissingLinks);
});
