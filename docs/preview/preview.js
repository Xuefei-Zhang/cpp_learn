import {
  buildStemIndex,
  normalizeRepoPath,
  resolveMarkdownHref,
  rewriteWikiLinksToHtml,
} from './preview-core.mjs';
import { marked } from 'https://cdn.jsdelivr.net/npm/marked/lib/marked.esm.js';
import mermaid from 'https://cdn.jsdelivr.net/npm/mermaid@11/dist/mermaid.esm.min.mjs';

const manifestUrl = './preview-manifest.json';
const docsTitleEl = document.getElementById('docs-title');
const navRootEl = document.getElementById('nav-root');
const navSearchEl = document.getElementById('nav-search');
const pageTitleEl = document.getElementById('page-title');
const pageMetaEl = document.getElementById('page-meta');
const pageStatusEl = document.getElementById('page-status');
const pageContentEl = document.getElementById('page-content');
const sidebarEl = document.getElementById('sidebar');
const sidebarToggleEl = document.getElementById('sidebar-toggle');

mermaid.initialize({ startOnLoad: false, securityLevel: 'loose', theme: 'dark' });

let manifest = null;
let knownFiles = new Set();
let stemIndex = new Map();
let pagesByFile = new Map();

function flattenSidebar(sidebarGroups) {
  const pages = [];
  for (const group of sidebarGroups) {
    for (const item of group.items ?? []) {
      pages.push(item);
    }
  }
  return pages;
}

function getCurrentPage() {
  const params = new URLSearchParams(window.location.search);
  const requested = params.get('page');
  return requested ? normalizeRepoPath(requested) : manifest.landing;
}

function setCurrentPage(file) {
  const next = new URL(window.location.href);
  next.searchParams.set('page', file);
  window.history.pushState({}, '', next);
  renderPage(file);
  updateActiveNav(file);
}

function updateActiveNav(activeFile) {
  document.querySelectorAll('[data-nav-file]').forEach((link) => {
    link.classList.toggle('active', link.dataset.navFile === activeFile);
  });
}

function renderNav(filter = '') {
  const lowered = filter.trim().toLowerCase();
  navRootEl.innerHTML = '';

  for (const group of manifest.sidebar) {
    const items = (group.items ?? []).filter((item) => item.title.toLowerCase().includes(lowered));
    if (lowered && items.length === 0) {
      continue;
    }

    const section = document.createElement('section');
    section.className = 'nav-group';

    const title = document.createElement('div');
    title.className = 'nav-group-title';
    title.textContent = group.title;
    section.appendChild(title);

    const list = document.createElement('ul');
    list.className = 'nav-list';

    for (const item of items) {
      const listItem = document.createElement('li');
      const link = document.createElement('a');
      link.href = `?page=${encodeURIComponent(item.file)}`;
      link.className = 'nav-link';
      link.dataset.navFile = item.file;
      link.textContent = item.title;
      link.addEventListener('click', (event) => {
        event.preventDefault();
        setCurrentPage(item.file);
      });
      listItem.appendChild(link);
      list.appendChild(listItem);
    }

    section.appendChild(list);
    navRootEl.appendChild(section);
  }

  updateActiveNav(getCurrentPage());
}

function collectBrokenLinkHints(markdown) {
  const matches = markdown.match(/<span class="broken-link"[^>]*>(.*?)<\/span>/g) ?? [];
  return matches.length;
}

async function renderMermaidBlocks(root) {
  const mermaidNodes = [...root.querySelectorAll('code.language-mermaid')];
  if (mermaidNodes.length === 0) {
    return;
  }

  for (const codeNode of mermaidNodes) {
    const source = codeNode.textContent ?? '';
    const wrapper = codeNode.closest('pre');
    if (!wrapper) {
      continue;
    }

    const container = document.createElement('div');
    container.className = 'mermaid';
    container.textContent = source;
    wrapper.replaceWith(container);
  }

  await mermaid.run({ querySelector: '.mermaid' });
}

async function renderPage(file) {
  const page = pagesByFile.get(file);
  pageTitleEl.textContent = page?.title ?? file;
  pageMetaEl.textContent = file;
  pageStatusEl.textContent = 'Loading markdown…';
  pageContentEl.innerHTML = '';

  try {
    const response = await fetch(`../../${file}`);
    if (!response.ok) {
      throw new Error(`HTTP ${response.status}`);
    }

    const rawMarkdown = await response.text();
    const rewritten = rewriteWikiLinksToHtml({
      markdown: rawMarkdown,
      currentFile: file,
      stemIndex,
      knownFiles,
    });

    const html = marked.parse(rewritten);
    pageContentEl.innerHTML = html;

    for (const anchor of pageContentEl.querySelectorAll('a[href]')) {
      const href = anchor.getAttribute('href') ?? '';
      const resolved = resolveMarkdownHref({ currentFile: file, href, knownFiles });
      if (!resolved) {
        continue;
      }
      anchor.href = `?page=${encodeURIComponent(resolved)}`;
      anchor.classList.add('internal-link');
      anchor.dataset.page = resolved;
    }

    for (const anchor of pageContentEl.querySelectorAll('a[data-page]')) {
      anchor.addEventListener('click', (event) => {
        event.preventDefault();
        setCurrentPage(anchor.dataset.page);
      });
    }

    const brokenCount = collectBrokenLinkHints(pageContentEl.innerHTML);
    pageStatusEl.textContent = brokenCount > 0
      ? `${brokenCount} unresolved future-doc link${brokenCount === 1 ? '' : 's'} shown inline.`
      : 'All in-scope links on this page resolve in the current preview set.';

    await renderMermaidBlocks(pageContentEl);
  } catch (error) {
    pageStatusEl.textContent = 'Failed to load page.';
    pageContentEl.innerHTML = `<div class="error-state">Could not load <code>${file}</code>: ${error.message}</div>`;
  }
}

async function init() {
  const response = await fetch(manifestUrl);
  manifest = await response.json();
  docsTitleEl.textContent = manifest.title;
  const pages = flattenSidebar(manifest.sidebar);
  knownFiles = new Set(pages.map((page) => page.file));
  stemIndex = buildStemIndex([...knownFiles]);
  pagesByFile = new Map(pages.map((page) => [page.file, page]));

  renderNav();
  await renderPage(getCurrentPage());

  navSearchEl.addEventListener('input', () => {
    renderNav(navSearchEl.value);
  });

  sidebarToggleEl.addEventListener('click', () => {
    const hidden = sidebarEl.classList.toggle('hidden');
    sidebarToggleEl.setAttribute('aria-expanded', String(!hidden));
  });

  window.addEventListener('popstate', () => {
    const current = getCurrentPage();
    updateActiveNav(current);
    renderPage(current);
  });
}

init().catch((error) => {
  pageTitleEl.textContent = 'Preview failed to initialize';
  pageStatusEl.textContent = error.message;
  pageContentEl.innerHTML = '<div class="error-state">The docs preview could not load its manifest.</div>';
});
