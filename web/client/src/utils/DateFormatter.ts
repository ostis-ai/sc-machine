export function DateToText(date: Date): string {
  // TODO: add today, yesterday prefixes
  return date.toLocaleTimeString();
}