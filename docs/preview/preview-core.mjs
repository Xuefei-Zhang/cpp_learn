function splitHash(input) {
  const hashIndex = input.indexOf('#');
  if (hashIndex === -1) {
    return { pathPart: input, hash: '' };
  }

  return {
    pathPart: input.slice(0, hashIndex),
    hash: input.slice(hashIndex),
  };
}

function normalizeSegments(pathValue) {
  const segments = String(pathValue)
    .replace(/\\/g, '/')
    .split('/');
  const normalized = [];

  for (const segment of segments) {
    if (!segment || segment === '.') {
      continue;
    }

    if (segment === '..') {
      if (normalized.length > 0 && normalized[normalized.length - 1] !== '..') {
        normalized.pop();
      } else {
        normalized.push('..');
      }
      continue;
    }

    normalized.push(segment);
  }

  return normalized;
}

function dirname(filePath) {
  const normalized = normalizeRepoPath(filePath);
  const lastSlash = normalized.lastIndexOf('/');
  return lastSlash === -1 ? '' : normalized.slice(0, lastSlash);
}

function joinRepoPath(baseDir, targetPath) {
  if (!baseDir) {
    return normalizeRepoPath(targetPath);
  }

  return normalizeRepoPath(`${baseDir}/${targetPath}`);
}

function hasExtension(pathValue) {
  const lastSegment = pathValue.split('/').pop() ?? '';
  return /\.[A-Za-z0-9_-]+$/.test(lastSegment);
}

function ensureMarkdownPath(pathValue) {
  return hasExtension(pathValue) ? pathValue : `${pathValue}.md`;
}

function isPathLike(target) {
  return target.startsWith('.') || target.startsWith('/') || target.includes('/');
}

function escapeHtml(value) {
  return String(value)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
    .replace(/'/g, '&#39;');
}

export function normalizeRepoPath(pathValue) {
  const normalized = normalizeSegments(pathValue).join('/');
  return normalized.replace(/^\//, '');
}

export function buildStemIndex(files) {
  const index = new Map();

  for (const file of files) {
    const normalized = normalizeRepoPath(file);
    const lastSegment = normalized.split('/').pop() ?? normalized;
    const stem = lastSegment.endsWith('.md') ? lastSegment.slice(0, -3) : lastSegment;
    const matches = index.get(stem) ?? [];
    matches.push(normalized);
    index.set(stem, matches);
  }

  return index;
}

export function resolveWikiTarget({ currentFile, targetRaw, stemIndex, knownFiles }) {
  const trimmedTarget = String(targetRaw).trim();
  const { pathPart, hash } = splitHash(trimmedTarget);

  if (!pathPart) {
    return { ok: false, reason: 'empty-target' };
  }

  if (isPathLike(pathPart)) {
    const currentDir = dirname(currentFile);
    const relativeTarget = pathPart.startsWith('/') ? pathPart.slice(1) : pathPart;
    const normalizedTarget = ensureMarkdownPath(joinRepoPath(currentDir, relativeTarget));

    if (!knownFiles.has(normalizedTarget)) {
      return { ok: false, reason: 'missing-target', attempted: normalizedTarget };
    }

    return { ok: true, path: `${normalizedTarget}${hash}` };
  }

  const stemMatches = stemIndex.get(pathPart) ?? [];
  if (stemMatches.length === 1) {
    return { ok: true, path: `${stemMatches[0]}${hash}` };
  }

  if (stemMatches.length > 1) {
    return { ok: false, reason: 'ambiguous-target', candidates: stemMatches };
  }

  const directMarkdown = ensureMarkdownPath(normalizeRepoPath(pathPart));
  if (knownFiles.has(directMarkdown)) {
    return { ok: true, path: `${directMarkdown}${hash}` };
  }

  return { ok: false, reason: 'missing-target', attempted: directMarkdown };
}

export function extractWikiLinksOutsideCodeFences(markdown) {
  const lines = String(markdown).split(/\r?\n/);
  const results = [];
  let inFence = false;

  for (const line of lines) {
    if (/^\s*(```|~~~)/.test(line)) {
      inFence = !inFence;
      continue;
    }

    if (inFence) {
      continue;
    }

    const matches = line.matchAll(/\[\[([^\]\n]+)\]\]/g);
    for (const match of matches) {
      const raw = match[1];
      const separatorIndex = raw.indexOf('|');
      const target = separatorIndex === -1 ? raw.trim() : raw.slice(0, separatorIndex).trim();
      const alias = separatorIndex === -1 ? null : raw.slice(separatorIndex + 1).trim();
      results.push({ raw, target, alias });
    }
  }

  return results;
}

export function rewriteWikiLinksToHtml({ markdown, currentFile, stemIndex, knownFiles }) {
  const lines = String(markdown).split(/\r?\n/);
  let inFence = false;

  return lines
    .map((line) => {
      if (/^\s*(```|~~~)/.test(line)) {
        inFence = !inFence;
        return line;
      }

      if (inFence) {
        return line;
      }

      return line.replace(/\[\[([^\]\n]+)\]\]/g, (_match, raw) => {
        const separatorIndex = raw.indexOf('|');
        const target = separatorIndex === -1 ? raw.trim() : raw.slice(0, separatorIndex).trim();
        const alias = separatorIndex === -1 ? '' : raw.slice(separatorIndex + 1).trim();
        const resolved = resolveWikiTarget({ currentFile, targetRaw: target, stemIndex, knownFiles });
        const label = escapeHtml(alias || target);

        if (!resolved.ok) {
          return `<span class="broken-link" title="${escapeHtml(resolved.reason)}">${label}</span>`;
        }

        const href = `?page=${encodeURIComponent(resolved.path)}`;
        return `<a class="internal-link" href="${href}" data-page="${escapeHtml(resolved.path)}">${label}</a>`;
      });
    })
    .join('\n');
}

export function resolveMarkdownHref({ currentFile, href, knownFiles }) {
  const trimmedHref = String(href).trim();
  const { pathPart, hash } = splitHash(trimmedHref);

  if (!pathPart || /^(?:[a-z]+:|\/\/)/i.test(pathPart) || pathPart.startsWith('#')) {
    return null;
  }

  const normalizedTarget = pathPart.startsWith('/')
    ? normalizeRepoPath(pathPart.slice(1))
    : joinRepoPath(dirname(currentFile), pathPart);

  if (!knownFiles.has(normalizedTarget)) {
    return null;
  }

  return `${normalizedTarget}${hash}`;
}
